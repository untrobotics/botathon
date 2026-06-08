#!/usr/bin/env node

var fs = require('fs');
var path = require('path');
var http = require('http');
var spawn = require('child_process').spawn;

var express = require('express');
var WebSocket = require('ws');

var PORT = parseInt(process.env.PORT || '81', 10);
var HLS_DIR = path.join(__dirname, 'hls');
var STATIC_DIR = path.resolve(__dirname, '..', 'web');

if (!fs.existsSync(HLS_DIR)) fs.mkdirSync(HLS_DIR, { recursive: true });

// per-team state. keyed by team number.
var teams = {};

function getTeam(id) {
	if (!teams[id]) teams[id] = newTeam(id);
	return teams[id];
}

function newTeam(id) {
	return {
		id: id,
		motorWs: null,
		cameraWs: null,
		driverWs: null,
		ffmpeg: null,
		viewers: [], // multipart MJPEG responses for the driver feed
	};
}

function startFfmpeg(team) {
	if (team.ffmpeg) {
		try { team.ffmpeg.kill('SIGKILL'); } catch (e) {}
		team.ffmpeg = null;
	}

	var teamHlsDir = path.join(HLS_DIR, 'team_' + team.id);
	if (!fs.existsSync(teamHlsDir)) fs.mkdirSync(teamHlsDir, { recursive: true });

	var args = [
		'-hide_banner', '-loglevel', 'warning',
		'-f', 'mjpeg', '-use_wallclock_as_timestamps', '1', '-i', 'pipe:0',

		// output 1: copy the MJPEG to stdout for the driver viewers (no re-encode)
		'-map', '0:v', '-c:v', 'copy',
		'-f', 'mjpeg', 'pipe:1',

		// output 2: HLS for the public spectator
		'-map', '0:v', '-c:v', 'libx264',
			'-preset', 'veryfast', '-tune', 'zerolatency',
			'-pix_fmt', 'yuv420p',
			'-g', '30', '-sc_threshold', '0',
		'-f', 'hls',
			'-hls_time', '2',
			'-hls_list_size', '4',
			'-hls_flags', 'delete_segments+independent_segments',
			'-hls_segment_filename', path.join(teamHlsDir, 'seg_%05d.ts'),
			path.join(teamHlsDir, 'index.m3u8'),
	];

	var proc = spawn('ffmpeg', args, { stdio: ['pipe', 'pipe', 'pipe'] });

	proc.stdout.on('data', function(chunk) {
		for (var i = 0; i < team.viewers.length; i++) {
			try { team.viewers[i].write(chunk); } catch (e) {}
		}
	});

	proc.stderr.on('data', function(chunk) {
		var line = chunk.toString().trim();
		if (line) console.log('[team ' + team.id + '] ffmpeg: ' + line);
	});

	proc.on('exit', function(code, signal) {
		console.log('[team ' + team.id + '] ffmpeg exit code=' + code + ' signal=' + signal);
		if (team.ffmpeg === proc) team.ffmpeg = null;
	});

	team.ffmpeg = proc;
}

function pushFrame(team, buf) {
	if (!team.ffmpeg || !team.ffmpeg.stdin.writable) return;
	if (team.ffmpeg.stdin.writableLength > 256 * 1024) return; // drop frame if we're backed up
	team.ffmpeg.stdin.write(buf);
}

// HTTP

var app = express();

app.use('/driver', express.static(path.join(STATIC_DIR, 'driver')));
app.use('/spectator', express.static(path.join(STATIC_DIR, 'spectator')));
app.get('/', function(req, res) { res.redirect('/spectator/'); });

// multipart MJPEG for the driver <img>
app.get('/mjpeg/team_:teamId', function(req, res) {
	var teamId = parseInt(req.params.teamId, 10);
	if (isNaN(teamId)) return res.status(400).end('bad team id');

	var team = getTeam(teamId);

	var boundary = 'botathonframe';
	res.writeHead(200, {
		'Content-Type': 'multipart/x-mixed-replace; boundary=' + boundary,
		'Cache-Control': 'no-store',
		'Connection': 'close',
		'Pragma': 'no-cache',
	});

	// ffmpeg gives us raw JPEGs back to back; wrap them with the multipart preamble
	var viewer = {
		write: function(chunk) {
			res.write('--' + boundary + '\r\nContent-Type: image/jpeg\r\nContent-Length: ' + chunk.length + '\r\n\r\n');
			res.write(chunk);
			res.write('\r\n');
		}
	};

	team.viewers.push(viewer);
	req.on('close', function() {
		var i = team.viewers.indexOf(viewer);
		if (i >= 0) team.viewers.splice(i, 1);
	});
});

// public HLS
app.use('/hls', function(req, res, next) {
	res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Cache-Control', 'no-store');
	next();
}, express.static(HLS_DIR));

var server = http.createServer(app);

// WebSocket

var wss = new WebSocket.Server({ noServer: true });

server.on('upgrade', function(req, socket, head) {
	var url = new URL(req.url, 'http://' + (req.headers.host || 'localhost'));
	var pathname = url.pathname;
	if (pathname === '/' || pathname === '/video' || pathname === '/driver') {
		wss.handleUpgrade(req, socket, head, function(ws) {
			ws._role = pathname === '/' ? 'motor' : pathname === '/video' ? 'camera' : 'driver';
			wss.emit('connection', ws, req);
		});
	} else {
		socket.destroy();
	}
});

wss.on('connection', function(ws) {
	var role = ws._role;
	var team = null;

	ws.on('message', function(data, isBinary) {
		// first message identifies the team. format depends on role:
		//   motor:  "ESP32_TEAM_100"
		//   camera: "CAMERA_TEAM_100"
		//   driver: {"hello":"driver","team":100}
		if (!team) {
			var text = data.toString();
			var teamId = null;
			var m = text.match(/TEAM_(\d+)/);
			if (m) {
				teamId = parseInt(m[1], 10);
			} else {
				try {
					var obj = JSON.parse(text);
					if (obj && typeof obj.team === 'number') teamId = obj.team;
				} catch (e) {}
			}
			if (teamId === null) {
				ws.close(1008, 'missing team id');
				return;
			}
			team = getTeam(teamId);

			if (role === 'motor') {
				if (team.motorWs) team.motorWs.close();
				team.motorWs = ws;
				console.log('[team ' + teamId + '] motor connected');
			} else if (role === 'camera') {
				if (team.cameraWs) team.cameraWs.close();
				team.cameraWs = ws;
				startFfmpeg(team);
				console.log('[team ' + teamId + '] camera connected');
			} else if (role === 'driver') {
				if (team.driverWs) team.driverWs.close();
				team.driverWs = ws;
				console.log('[team ' + teamId + '] driver connected');
			}
			return;
		}

		if (role === 'camera' && isBinary) {
			pushFrame(team, data);
		} else if (role === 'driver') {
			// forward driver button events to the motor controller
			if (team.motorWs && team.motorWs.readyState === WebSocket.OPEN) {
				team.motorWs.send(data);
			}
		} else if (role === 'motor') {
			// telemetry back to the driver
			if (team.driverWs && team.driverWs.readyState === WebSocket.OPEN) {
				team.driverWs.send(data);
			}
		}
	});

	ws.on('close', function() {
		if (!team) return;
		if (role === 'motor' && team.motorWs === ws) team.motorWs = null;
		if (role === 'camera' && team.cameraWs === ws) {
			team.cameraWs = null;
			if (team.ffmpeg) {
				try { team.ffmpeg.stdin.end(); } catch (e) {}
			}
		}
		if (role === 'driver' && team.driverWs === ws) team.driverWs = null;
		console.log('[team ' + team.id + '] ' + role + ' disconnected');
	});

	ws.on('error', function(err) {
		if (team) console.log('[team ' + team.id + '] ' + role + ' error: ' + err.message);
	});
});

server.listen(PORT, function() {
	console.log('botathon relay listening on :' + PORT);
});

function shutdown() {
	console.log('shutting down');
	for (var id in teams) {
		var t = teams[id];
		if (t.ffmpeg) { try { t.ffmpeg.kill('SIGKILL'); } catch (e) {} }
	}
	server.close(function() { process.exit(0); });
	setTimeout(function() { process.exit(1); }, 5000).unref();
}
process.on('SIGINT', shutdown);
process.on('SIGTERM', shutdown);
