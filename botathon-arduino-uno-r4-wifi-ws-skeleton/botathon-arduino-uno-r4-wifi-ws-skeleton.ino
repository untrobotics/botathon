#include <WiFi.h>
#include "Arduino.h"
#define NETWORK_CONTROLLER NETWORK_CONTROLLER_WIFI
#include <WebSocketClient.h> // install mWebSockets library
using namespace net; // do not remove!

#include "ArduinoGraphics.h" // install ArduinoGraphics library
#include "Arduino_LED_Matrix.h" // install ArduinoLEDMatrix library (usually comes pre-installed) 

#include <ArduinoJson.h>

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

StaticJsonDocument<200> doc;

// Motor A connections
int enA = 9; //9
int in1 = 8; //8
int in2 = 7; //7
// Motor B connections
int enB = 3; //3
int in3 = 5; //5
int in4 = 4; //4

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

bool getJSON(WebSocket &ws, String message);

void motorSetUp(){
  Serial.begin(115200);
  Serial.setTimeout(100);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  Serial.println("Started");
}
bool A_BUTTON_PRESSED = false;
void control(){
  auto input = doc["input"];
  auto key = input["key"];
  auto value = input["value"];
     if (key == "A_BUTTON") {
      if (value == true) {
        A_BUTTON_PRESSED = !A_BUTTON_PRESSED;
        Serial.print("Direction switched:");
        Serial.println(A_BUTTON_PRESSED);
      }
    }

    if (key == "RIGHT_TRIGGER") {
      if (value > -1) {
        if (A_BUTTON_PRESSED) {
          right_forwards();
        } else {
          right_backwards();
        }
      } else {
        stop_right();
      }
    }

    if (key == "LEFT_TRIGGER") {
      if (value > -1) {
        if (A_BUTTON_PRESSED) {
          left_forwards(); 
        } else {
          left_backwards();
        }
      } else {
        stop_left();
      }
    }
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
    if(getJSON(ws, message)==false){
      return;
    }
    
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
bool getJSON(WebSocket &ws, String message){
  String command = String(message);
  if (deserializeJson(doc, command)) {
    Serial.println("Error parsing message");
    return false;
  }
  char s[20];
  sprintf(s, "^%d", doc["sequenceNumber"].as<int>());
  ws.send(WebSocket::DataType::TEXT, s, strlen(s));
  return true;
}


void forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}
void backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void left_forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}
void right_forwards() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void left_backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}
void right_backwards() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stop_left() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void stop_right() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control spinning direction of motors
void directionControl() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  // Turn on motor A & B
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(2000);
  
  // Now change motor directions
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(2000);
  
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
  // Turn on motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
