/*
[0;32mI (42) boot.esp32: SPI Speed      : 40MHz[0m
[0;32mI (47) boot.esp32: SPI Mode       : DIO[0m
[0;32mI (51) boot.esp32: SPI Flash Size : 4MB[0m
*/

//#include "esp_wpa2.h"

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" // disable brownout problems
#include "soc/rtc_cntl_reg.h"  //  disable brownout problems
#include "esp_http_server.h"
#include <WebSocketsClient.h>
#include <WiFiUdp.h>

//WiFiClient client;

//#define EAP_IDENTITY "secret"
//#define EAP_PASSWORD "secret"

WiFiUDP Udp;
WebSocketsClient webSocket;

//const char* ssid = "UNT";
const char* ssid = "secret";
const char* password = "secret";

#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected"
#endif

#define MAX_FRAGMENT_SIZE 1400 // this needs to be less than your MTU size, otherwise the receiving server will timeout waiting for the fragmented packet
#define ONBOARD_LED 33
#define FLASH_LED 4

int TEAM_NO = 107;

TaskHandle_t Task1;
TaskHandle_t Task2;

uint32_t prevTimestamp = 0;
u_short sequenceNumber = 0;
uint32_t convertedTimestamp = 0;

static void sendData(uint8_t * payload, size_t len) {

  int fragmentOffset = 0;
  bool isLastFragment;

  uint32_t timestamp = millis();
  uint32_t deltams = (timestamp >= prevTimestamp) ? timestamp - prevTimestamp : 100;
  prevTimestamp = timestamp;
  uint32_t units = 90000; // Hz per RFC 2435
  convertedTimestamp += (units * deltams / 1000);  
    
  do {
    int fragmentLen = MAX_FRAGMENT_SIZE;
    if (fragmentLen + fragmentOffset > len) {
      fragmentLen = len - fragmentOffset;
    }
    isLastFragment = (fragmentOffset + fragmentLen) == len;
    
    //Serial.printf("Sending timestamp %d, seq %d, fragoff %d, fraglen %d, jpegLen %d, last %d\n", convertedTimestamp, sequenceNumber, fragmentOffset, fragmentLen, len, isLastFragment);
    
    int headerLen = 20;
    static uint8_t rtpBuf[MAX_FRAGMENT_SIZE + 20];
    
    //int packetLen = len + headerLen;
    //uint8_t * rtpPayload = payload - headerLen;
    
    memset(rtpBuf, 0x00, sizeof(rtpBuf));
    
    // Prepare the 12 byte RTP header
    rtpBuf[0] = 0x80;                                        // RTP version (2)
    rtpBuf[1] = 0x1a | (isLastFragment ? 0x80 : 0x00);       // JPEG payload (26) and marker bit (review)
    
    rtpBuf[3] = sequenceNumber & 0x0FF;                      // each packet is counted with a sequence counter
    rtpBuf[2] = sequenceNumber >> 8;
    
    rtpBuf[4] = (convertedTimestamp & 0xFF000000) >> 24;     // each image gets a timestamp, this must be consistent for the whole frame regardless of fragment
    rtpBuf[5] = (convertedTimestamp & 0x00FF0000) >> 16;
    rtpBuf[6] = (convertedTimestamp & 0x0000FF00) >> 8;
    rtpBuf[7] = (convertedTimestamp & 0x000000FF);
    
    rtpBuf[8] = 0x13;                                       // 4 byte SSRC (sychronization source identifier)
    rtpBuf[9] = 0xf9;                                       // we just an arbitrary number here to keep it simple
    rtpBuf[10] = 0x7e;
    rtpBuf[11] = 0x67;
  
    // Prepare the 8 byte JPEG header
    rtpBuf[12] = 0x00;                                      // type specific    
    rtpBuf[13] = (fragmentOffset & 0x00FF0000) >> 16;       // 3 byte fragmentation offset for fragmented images
    rtpBuf[14] = (fragmentOffset & 0x0000FF00) >> 8;
    rtpBuf[15] = (fragmentOffset & 0x000000FF);
    
    rtpBuf[16] = 0x00;                                      // type -- depends on config.pixel_format: https://tools.ietf.org/html/rfc2435 / https://github.com/espressif/esp32-camera/blob/master/driver/include/sensor.h
    rtpBuf[17] = 0x3f;//0x3f;                               // quality scale factor was 0x5e (what?)
    rtpBuf[18] = 800 / 8;                                   // width  / 8
    rtpBuf[19] = 600 / 8;                                   // height / 8
  
    //memcpy(RtpBuf + headerLen, payload, packetLen);
    //memcpy(rtpPayload, rtpBuf, packetLen);
    //memcpy(rtpBuf + 12, payload, len);
    memcpy(rtpBuf + headerLen, payload + fragmentOffset, fragmentLen);
  
    sequenceNumber++;
  
    Udp.beginPacket("192.168.254.16", 2115);
    //Udp.write(rtpBuf, headerLen);
    //Udp.write(payload, len);
    Udp.write(rtpBuf, fragmentLen + headerLen);
    Udp.endPacket();

    fragmentOffset += fragmentLen;
    
  } while (!isLastFragment);
}

static void capture(){
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  
  if (!fb) {
    Serial.println("CAMERA_CAPTURE_FAIL");
  } else {
    sendData(fb->buf, fb->len);
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
    }
  }
}

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
      //Serial.printf("COMMAND: '%s'\n", command);
      Serial.print(command);

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
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(!psramFound()){
    //config.frame_size = FRAMESIZE_UXGA;
    //config.jpeg_quality = 10;
    //config.fb_count = 2;
    Serial.println("ERR_PSRAM_NOT_FOUND");
    while (1);
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 63;
    config.fb_count = 2;
  }
  
  // Camera init
  /*
  esp_err_t err = esp_camera_init(&config);
  while (err != ESP_OK) {
    Serial.printf("ERR_CAMERA_INIT_FAIL_0x%x\n", err);
    err = esp_camera_init(&config);
  }
  */
  
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

void camera(void * pvParameters) {
  //Serial.print("Task1 running on core ");
  //Serial.println(xPortGetCoreID());

  while(1) {
    //delay(1000);
    total_time = millis();
    capture();
    total_time = millis() - total_time;
    
    //Serial.print(F("Frame time (in miliseconds):"));
    //Serial.println(total_time, DEC);
  }
}

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
