#include <WiFi.h>
#include "Arduino.h"
#define NETWORK_CONTROLLER NETWORK_CONTROLLER_WIFI
#include <WebSocketClient.h> // install mWebSockets library
using namespace net; // do not remove!

WebSocketClient client;

const char* ssid = "UNT Robotics Botathon";
const char* password = "untrobotics";

int TEAM_NO = 100;

int total_time = 0;

void setup() {
  delay(2000);

  Serial.begin(115200);
  Serial.println("Setting up Arduino...");

  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.onOpen([](WebSocket &ws) {
    Serial.println("WEBSOCKET_CONNECTED");
    // send message to server when connected
    char s[14];
    sprintf(s, "ESP32_TEAM_%d", TEAM_NO);
    ws.send(WebSocket::DataType::TEXT, s, strlen(s));
  });
  client.onClose([](WebSocket &ws, const WebSocket::CloseCode code,
                   const char *reason, uint16_t length) {
    Serial.println("WEBSOCKET_DISCONNECTED");
  });
  client.onMessage([](WebSocket &ws, const WebSocket::DataType dataType,
                     const char *message, uint16_t length) {
    Serial.print("WEBSOCKET_MESSAGE_RECEIVED: ");
    Serial.println(message);
  });

  bool opened = client.open("untrobotics.com", 81, "/", "team");

  if (!opened) {
    Serial.println(F("Connection failed! Please restart the Arduino."));
  }
}

void loop() {
  // nowt
  client.listen();
}
