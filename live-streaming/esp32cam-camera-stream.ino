#include <WiFi.h>
#include <WebSocketsClient.h>
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// AI Thinker ESP32-CAM pinout
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

#define ONBOARD_LED 33
#define FLASH_LED    4

int TEAM_NO = 100; // set this per robot

const char* ssid = "UNT Robotics Botathon";
const char* password = "untrobotics";

WebSocketsClient webSocket;

bool wsConnected = false;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t len) {
  switch (type) {
    case WStype_CONNECTED: {
      wsConnected = true;
      char s[20];
      sprintf(s, "CAMERA_TEAM_%d", TEAM_NO);
      webSocket.sendTXT(s);
      Serial.printf("WS_CONNECTED: %s\n", s);
      break;
    }
    case WStype_DISCONNECTED:
      wsConnected = false;
      Serial.println("WS_DISCONNECTED");
      break;
    case WStype_TEXT: {
      String cmd = String((char*)payload);
      if (cmd == "FLASH_ON")  digitalWrite(FLASH_LED, HIGH);
      else if (cmd == "FLASH_OFF") digitalWrite(FLASH_LED, LOW);
      break;
    }
    default:
      break;
  }
}

void initCamera() {
  camera_config_t cfg;
  cfg.ledc_channel = LEDC_CHANNEL_0;
  cfg.ledc_timer   = LEDC_TIMER_0;
  cfg.pin_d0       = Y2_GPIO_NUM;
  cfg.pin_d1       = Y3_GPIO_NUM;
  cfg.pin_d2       = Y4_GPIO_NUM;
  cfg.pin_d3       = Y5_GPIO_NUM;
  cfg.pin_d4       = Y6_GPIO_NUM;
  cfg.pin_d5       = Y7_GPIO_NUM;
  cfg.pin_d6       = Y8_GPIO_NUM;
  cfg.pin_d7       = Y9_GPIO_NUM;
  cfg.pin_xclk     = XCLK_GPIO_NUM;
  cfg.pin_pclk     = PCLK_GPIO_NUM;
  cfg.pin_vsync    = VSYNC_GPIO_NUM;
  cfg.pin_href     = HREF_GPIO_NUM;
  cfg.pin_sccb_sda = SIOD_GPIO_NUM;
  cfg.pin_sccb_scl = SIOC_GPIO_NUM;
  cfg.pin_pwdn     = PWDN_GPIO_NUM;
  cfg.pin_reset    = RESET_GPIO_NUM;
  cfg.xclk_freq_hz = 20000000;
  cfg.pixel_format = PIXFORMAT_JPEG;
  cfg.frame_size   = FRAMESIZE_VGA;
  cfg.jpeg_quality = 12;

  if (psramFound()) {
    cfg.fb_count = 2;
    cfg.fb_location = CAMERA_FB_IN_PSRAM;
    cfg.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    cfg.fb_count = 1;
    cfg.fb_location = CAMERA_FB_IN_DRAM;
    cfg.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    Serial.println("WARN_NO_PSRAM");
  }

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("ERR_CAMERA_INIT_0x%x\n", err);
    while (true) delay(1000);
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout

  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);

  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println("ESP32CAM_INIT");

  initCamera();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ONBOARD_LED, HIGH);
    delay(250);
    digitalWrite(ONBOARD_LED, LOW);
    delay(250);
  }
  Serial.println(WiFi.localIP());

  webSocket.begin("untrobotics.com", 81, "/video", "video");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);
}

void loop() {
  webSocket.loop();

  if (!wsConnected) {
    delay(5);
    return;
  }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("ERR_FB_GET");
    delay(10);
    return;
  }

  // send the JPEG as a binary WS frame; relay pipes it straight into ffmpeg stdin
  webSocket.sendBIN(fb->buf, fb->len);
  esp_camera_fb_return(fb);

  delay(33); // ~30fps cap
}
