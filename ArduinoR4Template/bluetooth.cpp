// This file doesn't need to be changed to get your bot working!
#include "bluetooth.h"

// this animation shows an X on the Arduino matrix. It's shown if an error occurs and the bot couldn't be initialized
const uint32_t anim_x[3] = {
		0x60670e39,
		0xc1f81f83,
		0x9c70e606
};

// initializes Bluetooth settings on the Arduino
// matrix is only passed to display errors/messages
void bluetoothInit(ArduinoLEDMatrix &matrix, BLEService &controlService, BLEUnsignedIntCharacteristic &controlCharacteristic){
  Serial.println("Starting BLE module");
   
  // begin bluetooth module initialization. BLE.begin() returns false if initialization fails
  if (!BLE.begin()) {
    Serial.println("starting BLE module failed!");

    // display error message
    #ifdef DEBUG_MATRIX
    matrix.println("   Failed to start BLE   ");
    matrix.endText(SCROLL_LEFT);
    #endif
    matrix.loadFrame(anim_x);
    // halt program execution indefinitely
    while (1);
  }

  // sets the name advertised on Bluetooth... this is how the computer will know which Arduino to connect yo
  BLE.setLocalName(TEAM_NAME);

  // set the UUID (ID) for the service this peripheral advertises:
  BLE.setAdvertisedService(controlService);

  // add the characteristic to the service
  controlService.addCharacteristic(controlCharacteristic);

  // add the service
  BLE.addService(controlService);

  // start advertising... advertising broadcasts the device across Bluetooth so that other devices can connect to it
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");

  #ifdef DEBUG_MATRIX
    matrix.println("   Bluetooth active.   ");
    matrix.endText(SCROLL_LEFT);
  #endif
}

// switches Arduino to pairing mode
void startPairing(){
  // startTime = millis();
  BLE.setPairable(Pairable::ONCE);
}

// disables pairing mode
void stopPairing(){
  BLE.setPairable(false);
}

// tells you if the Arduino is in pairing mode
bool isPairing(){
  return BLE.pairable();
}