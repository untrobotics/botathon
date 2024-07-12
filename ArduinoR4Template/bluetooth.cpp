#include "bluetooth.h"
const uint32_t anim_x[3] = {
		0x60670e39,
		0xc1f81f83,
		0x9c70e606
};
void bluetoothInit(ArduinoLEDMatrix &matrix, BLEService &controlService, BLEUnsignedIntCharacteristic &controlCharacteristic){
  Serial.println("Starting BLE module");
   
  // begin bluetooth module initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE module failed!");

    #ifdef DEBUG_MATRIX
    matrix.println("   Failed to start BLE   ");
    matrix.endText(SCROLL_LEFT);
    #endif
    matrix.loadFrame(anim_x);
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
}

void startPairing(){
  // startTime = millis();
  BLE.setPairable(Pairable::ONCE);
}
void stopPairing(){
  BLE.setPairable(false);
}
bool isPairing(){
  return BLE.pairable();
}