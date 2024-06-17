#include <ArduinoBLE.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "botathonValues.h"
// uncomment to have the LED matrix show debugging messages
#define DEBUG_MATRIX

// change team name as needed
#define TEAM_NAME "BotathonTeam23"
#define TEAM_NO   23  //used for the LED matrix
// change service ID to be unique
#define SERVICE_ID "1ae49b08-b750-4ef7-afd8-5395763c0da6"

BLEService controlService(SERVICE_ID);
BLEUnsignedIntCharacteristic controlCharacteristic(CHARACTERISTIC_ID, BLERead | BLEWrite);

void setup() {
  // start serial
  Serial.begin(BAUD_RATE);
  while (!Serial);
  //start LED matrix
  matrix.begin();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(40);
  matrix.textFont(Font_5x7);

  #ifdef DEBUG_MATRIX
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println("   Starting BLE module   ");
    matrix.endText(SCROLL_LEFT);
  #endif
  
  Serial.println("Starting BLE module");
  // begin bluetooth module initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE module failed!");

    #ifdef DEBUG_MATRIX
    matrix.println("   Failed to start BLE   ");
    matrix.endText(SCROLL_LEFT);
    #endif

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName(TEAM_NAME);
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(controlService);

  // add the characteristics to the service
  controlService.addCharacteristic(controlCharacteristic);

  // add the service
  BLE.addService(controlService);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");

  #ifdef DEBUG_MATRIX
    matrix.println("   Bluetooth active.   ");
    matrix.endText(SCROLL_LEFT);
  #endif
  
  matrix.println(TEAM_NO);
  matrix.endText();
}
unsigned int previousValue = 0;
void loop() {
  if(controlCharacteristic.written()){
    unsigned int currentValue = controlCharacteristic.value();
    if(previousValue == currentValue) {
      Serial.println("No change");
      return;}
    Serial.println(currentValue);



    previousValue = currentValue;
  }
}
