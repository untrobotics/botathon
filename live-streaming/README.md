# Botathon Live Streaming

This is the bit of the Botathon that let drivers compete from home during COVID. The robots stayed on campus and the drivers sat at home, and this code is what handled the video and controls over the internet.

The motor controllers, LED scoreboard and other robot code are in the parent directory of the repo. This subdirectory is just the camera streaming, the central relay, and the driver/spectator web pages.

## What's in here

* `esp32cam-camera-stream.ino` is the sketch that ran on the ESP32-CAM. It captures JPEG frames off the OV2640 and sends them as binary WebSocket frames to the relay. It is the counterpart to `../esp32-botathon.ino` which handled the control loop and flash LED on the same board.
* `server/` is the Node.js relay that ran in the cloud. It handles all the WebSocket connections (motor controllers, cameras, and drivers) and routes everything through ffmpeg.
* `web/driver/` is the page each driver loaded on their laptop. It shows the camera feed and reads gamepad inputs.
* `web/spectator/` is the public live stream page that anyone could watch the competition on.

## How it works

Every robot opens two outbound WebSocket connections to the relay, one for the motor controller (`../botathon-arduino-uno-r4-wifi-ws-full.ino`) and one for the camera. The motor controller listens for JSON button events from the driver and translates them into motor directions. The camera grabs JPEG frames off the OV2640 and sends them as binary WebSocket frames.

Each driver loads up the driver page on their own computer, enters their team number, and gets paired with their robot by the relay. The video feed comes through as multipart MJPEG straight into an `<img>` tag, which gives us sub-second latency because nothing is being re-encoded on the way. The driver's controller inputs are read with the Gamepad API at 30Hz and sent over a WebSocket back to the relay, which forwards them to the motor controller.

The public spectator stream is the same MJPEG feed but routed through ffmpeg to produce an HLS stream that can be served as static files. We did this so it would scale to a lot of viewers without making the relay itself work harder. The trade-off is that spectators see the action with a few seconds of delay, but they're not driving so it doesn't matter.

## Why we did it this way

The biggest constraint was network setup. The robots needed to be on the campus WiFi and the drivers were behind their home routers, so the only thing that works without manual port forwarding or NAT punching is having both sides open outbound connections to a relay in the middle. That's the relay server.

The other constraint was that the contest had to scale to enough viewers that the school could stream the final round, but we didn't want to pay for a CDN or fight with WebRTC. MJPEG over WebSocket for the drivers and HLS for everyone else turned out to be a good compromise. The drivers get the latency they need, and the static HLS segments can sit behind any web server or CDN if the audience gets big.

## Running the relay

You need Node 18 or newer and ffmpeg available on the path.

```
cd server
npm install
PORT=81 node server.js
```

The relay listens on one port for all the WebSocket roles and the static web pages. In production we ran it behind nginx with TLS so the gamepad page could be loaded over HTTPS, which most browsers require before they'll expose the Gamepad API.

## Flashing a robot

You need to set `TEAM_NO` to the same number in both the camera sketch (`esp32cam-camera-stream.ino`) and the motor sketch you're using. The relay uses that number to pair the motor connection, the camera connection and the driver session together.

The motor sketch uses the [mWebSockets](https://github.com/skaarj1989/mWebSockets) library and the camera sketch uses [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets), which both need to be installed via the Arduino library manager before you flash anything.
