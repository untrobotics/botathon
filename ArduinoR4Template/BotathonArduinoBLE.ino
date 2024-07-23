
#include "bluetooth.h"

ArduinoLEDMatrix matrix;

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
  
  bluetoothInit(matrix, controlService, controlCharacteristic);
  startPairing();
  matrix.println(TEAM_NO);
  matrix.endText();
  controlCharacteristic.writeValue(25);
}
unsigned int previousValue = 0;
void loop() {
  BLEDevice central = BLE.central();
  if(isPairing()){
    if(central.connected()){
      stopPairing();
    } else{
      return;
    }
  } else if(!central.connected()){
    startPairing();
    return;
  }

  if(controlCharacteristic.written()){
    unsigned int currentValue = controlCharacteristic.value();
    // insert your code here

    previousValue = currentValue;
  }
}