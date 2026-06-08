var team_input = document.getElementById('team');
var connect_btn = document.getElementById('connect');
var link_state = document.getElementById('link-state');
var video = document.getElementById('video');
var flash_btn = document.getElementById('flash');
var telemetry = document.getElementById('telemetry-log');

var ws = null;
var poll_timer = null;
var button_state = {};

// gamepad button indices -> motor controller keys
var button_map = {
	0: 'A_BUTTON',
	1: 'B_BUTTON',
	4: 'LEFT_BUMPER',
	5: 'RIGHT_BUMPER',
};

function set_state(state, text) {
	link_state.className = 'state state-' + state;
	link_state.textContent = text;
}

function connect() {
	var team_id = parseInt(team_input.value, 10);
	if (isNaN(team_id) || team_id < 1) {
		alert('Enter a team number');
		return;
	}

	disconnect();

	var proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
	ws = new WebSocket(proto + '//' + location.host + '/driver');

	ws.onopen = function() {
		set_state('up', 'connected');
		ws.send(JSON.stringify({ hello: 'driver', team: team_id }));
		flash_btn.disabled = false;
		video.src = '/mjpeg/team_' + team_id;
		start_polling();
	};

	ws.onmessage = function(ev) {
		append_telemetry(typeof ev.data === 'string' ? ev.data : '[binary]');
	};

	ws.onclose = function() {
		set_state('down', 'disconnected');
		flash_btn.disabled = true;
		stop_polling();
	};

	ws.onerror = function() {
		set_state('down', 'error');
	};
}

function disconnect() {
	if (ws) {
		try { ws.close(); } catch (e) {}
		ws = null;
	}
	stop_polling();
	video.removeAttribute('src');
	flash_btn.disabled = true;
	button_state = {};
}

function start_polling() {
	stop_polling();
	poll_timer = setInterval(poll_gamepad, 1000 / 30);
}

function stop_polling() {
	if (poll_timer !== null) clearInterval(poll_timer);
	poll_timer = null;
}

function poll_gamepad() {
	if (!ws || ws.readyState !== WebSocket.OPEN) return;
	var pads = navigator.getGamepads ? navigator.getGamepads() : [];
	var pad = null;
	for (var i = 0; i < pads.length; i++) {
		if (pads[i]) { pad = pads[i]; break; }
	}
	if (!pad) return;

	for (var idx in button_map) {
		var key = button_map[idx];
		var btn = pad.buttons[parseInt(idx, 10)];
		if (!btn) continue;
		var pressed = !!btn.pressed;
		var prev = button_state[key] || false;
		if (pressed !== prev) {
			button_state[key] = pressed;
			ws.send(JSON.stringify({ key: key, value: pressed }));
			//console.log('SENT', key, pressed);
		}
	}
}

function send_flash(on) {
	if (!ws || ws.readyState !== WebSocket.OPEN) return;
	ws.send(on ? 'FLASH_ON' : 'FLASH_OFF');
}

function append_telemetry(line) {
	var current = telemetry.textContent;
	var next = (current === '(none yet)' ? '' : current + '\n') + line;
	var lines = next.split('\n');
	telemetry.textContent = lines.slice(-50).join('\n');
}

connect_btn.addEventListener('click', connect);
flash_btn.addEventListener('mousedown',  function() { send_flash(true); });
flash_btn.addEventListener('mouseup',    function() { send_flash(false); });
flash_btn.addEventListener('mouseleave', function() { send_flash(false); });
window.addEventListener('beforeunload',  disconnect);
