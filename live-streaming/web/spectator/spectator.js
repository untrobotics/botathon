var team_input = document.getElementById('team');
var watch_btn = document.getElementById('watch');
var player = document.getElementById('player');

var hls = null;

function watch() {
	var team_id = parseInt(team_input.value, 10);
	if (isNaN(team_id) || team_id < 1) {
		alert('Enter a team number');
		return;
	}

	if (hls) { hls.destroy(); hls = null; }
	var src = '/hls/team_' + team_id + '/index.m3u8';

	if (player.canPlayType('application/vnd.apple.mpegurl')) {
		// Safari has native HLS
		player.src = src;
	} else if (window.Hls && window.Hls.isSupported()) {
		hls = new window.Hls({
			liveSyncDuration: 2,
			maxBufferLength: 6,
			lowLatencyMode: true,
		});
		hls.loadSource(src);
		hls.attachMedia(player);
	} else {
		alert('Your browser does not support HLS playback.');
	}
}

watch_btn.addEventListener('click', watch);

// allow ?team=N in the URL to auto-start watching
var params = new URLSearchParams(location.search);
if (params.has('team')) {
	team_input.value = params.get('team');
	watch();
}
