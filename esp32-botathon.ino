#include <WiFi.h>
#include "esp_timer.h"
#include "Arduino.h"
#include "soc/soc.h" // disable brownout problems
#include "soc/rtc_cntl_reg.h"  //  disable brownout problems
#include <WebSocketsClient.h>

/*
ESP32s
Controllers
Lighting
Ref board
Timer
Wifi
Sample robot code
 */

WebSocketsClient webSocket;

//const char* ssid = "UNT";
const char* ssid = "UNT Robotics Botathon";
const char* password = "wearerobotics";

#define ONBOARD_LED 33
#define FLASH_LED 4

int TEAM_NO = 100;

TaskHandle_t Task1;
TaskHandle_t Task2;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t len) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("WEBSOCKET_DISCONNECTED\n");
      break;
    case WStype_CONNECTED:
    {
      Serial.printf("WEBSOCKET_CONNECTED: %s\n", payload);

      String teamNumber = "TEAM_";
      
      // send message to server when Connected
      char s[8];
      sprintf(s, "ESP32_TEAM_%d", TEAM_NO);

      //const char * teamNumber = "TEAM_".concat(TEAM_NO);
      webSocket.sendTXT(s);
      
      break;
    }
    case WStype_TEXT:
    {
      //Serial.printf("WEBSOCKET_RECEIVED_S: %s\n", payload);
      String command = String((char *)payload);
      Serial.print("COMMAND: ");
      Serial.println(command);
      Serial1.println(command);

      if (command == "FLASH_ON") {
        digitalWrite(FLASH_LED, HIGH);
      } else if (command == "FLASH_OFF") {
        digitalWrite(FLASH_LED, LOW);
      }

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    }
    case WStype_BIN:
      Serial.printf("WEBSOCKET_RECEIVED_B: %u\n", len);
      //hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:
      Serial.printf("WEBSOCKET_ERROR\n");
      break;
    case WStype_FRAGMENT_TEXT_START:
      Serial.printf("WEBSOCKET_FRAGMENT_TEXT_START\n");
      break;
    case WStype_FRAGMENT_BIN_START:
      Serial.printf("WEBSOCKET_FRAGMENT_BIN_START\n");
      break;
    case WStype_FRAGMENT:
      Serial.printf("WEBSOCKET_FRAGMENT\n");
      break;
    case WStype_FRAGMENT_FIN:
      Serial.printf("WEBSOCKET_FRAGMENT_FIN\n");
      break;
  }
}

int total_time = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32_INIT");
  Serial.setDebugOutput(false);
  
  if(!psramFound()){
    Serial.println("ERR_PSRAM_NOT_FOUND");
  }
  
  // Wi-Fi connection

  /*
  WiFi.disconnect(true);
  delay(10);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t wifiConfig = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&wifiConfig);
  */

  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
    digitalWrite(ONBOARD_LED, HIGH);
    delay(250);
    digitalWrite(ONBOARD_LED, LOW);
  }
  
  //Serial.printf("IP_%s\n", WiFi.localIP().toString());
  Serial.println(WiFi.localIP());

  webSocket.begin("untrobotics.com", 9111, "/", "team");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

//  xTaskCreatePinnedToCore(
//                    camera,   /* Task function. */
//                    "CAM",       /* name of task. */
//                    10000,       /* Stack size of task */
//                    NULL,        /* parameter of the task */
//                    1,           /* priority of the task */
//                    &Task1,      /* Task handle to keep track of created task */
//                    0);          /* pin task to core 0 */

  xTaskCreatePinnedToCore(
                    control,   /* Task function. */
                    "CONTROL",   /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
}

//void camera(void * pvParameters) {
//  //Serial.print("Task1 running on core ");
//  //Serial.println(xPortGetCoreID());
//
//  while(1) {
//    //delay(1000);
//    total_time = millis();
//    capture();
//    total_time = millis() - total_time;
//    
//    //Serial.print(F("Frame time (in miliseconds):"));
//    //Serial.println(total_time, DEC);
//  }
//}

void control(void * pvParameters) {
  //Serial.print("Task2 running on core ");
  //Serial.println(xPortGetCoreID());

  while (1) {
    digitalWrite(ONBOARD_LED, HIGH);
    delay(1000);
    digitalWrite(ONBOARD_LED, LOW);
    delay(1000);
  }
}

void loop() {
  // nowt
  webSocket.loop();
}
