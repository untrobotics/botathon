#include <WiFi.h>
#include "Arduino.h"
#define NETWORK_CONTROLLER NETWORK_CONTROLLER_WIFI
#include <WebSocketClient.h> // install mWebSockets library
using namespace net; // do not remove!

#include "ArduinoGraphics.h" // install ArduinoGraphics library
#include "Arduino_LED_Matrix.h" // install ArduinoLEDMatrix library (usually comes pre-installed) 

ArduinoLEDMatrix matrix;

WebSocketClient client;

const char* ssid = "UNT Robotics Botathon";
const char* password = "untrobotics";

const char* websocket_host = "untrobotics.com";
const int websocket_port = 81;
const char* websocket_path = "/";
const char* websocket_protocol = "team";

int TEAM_NUM = 100;

int status_indicator_pin = 13;

void displayTeamNumber() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  char team_num_cstr[3];
  sprintf (team_num_cstr, "%03i", TEAM_NUM);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(team_num_cstr);
  matrix.endText();

  matrix.endDraw();
}

void displayConnected() {
  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  // add the text
  const char text[] = "WiFi Connected";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}

void setup() {
  delay(2000);

  Serial.begin(115200);
  Serial.println("Setting up Arduino...");

  pinMode(status_indicator_pin, OUTPUT);

  // LED matrix config
  matrix.begin();

  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  displayConnected();
  displayTeamNumber();
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("You are team #");
  Serial.println(TEAM_NUM);

  client.onOpen([](WebSocket &ws) {
    Serial.println("WEBSOCKET_CONNECTED");
    // send message to server when connected
    char s[14];
    sprintf(s, "ESP32_TEAM_%d", TEAM_NUM);
    ws.send(WebSocket::DataType::TEXT, s, strlen(s));
    
    // indicate connected status
    digitalWrite(status_indicator_pin, HIGH);
  });
  client.onClose([](WebSocket &ws, const WebSocket::CloseCode code,
                   const char *reason, uint16_t length) {
    // indicate disconnected status
    digitalWrite(status_indicator_pin, LOW);

    Serial.println("WEBSOCKET_DISCONNECTED");
    client.terminate();

    while (!client.open(websocket_host, websocket_port, websocket_path, websocket_protocol)) {
      Serial.println("Attempting to reconnect...");
      Serial.println(F("WebSocket connection failed!"));

      if (WiFi.status() != WL_CONNECTED) {
        // flash the LED to indicate that the WiFi is disconnected
        digitalWrite(status_indicator_pin, HIGH);
        delay(250);
        digitalWrite(status_indicator_pin, LOW);
      }
      delay(2000);
    }
  });
  client.onMessage([](WebSocket &ws, const WebSocket::DataType dataType,
                     const char *message, uint16_t length) {
    Serial.print("WEBSOCKET_MESSAGE_RECEIVED: ");
    Serial.println(message);
  });

  while (!client.open(websocket_host, websocket_port, websocket_path, websocket_protocol)) {
    Serial.println(F("WebSocket connection failed!"));
    delay(2000);
  }
}

void loop() {
  // nowt
  client.listen();
}
