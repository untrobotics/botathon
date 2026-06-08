// This file doesn't need to be changed to get your bot working!
#include <ArduinoBLE.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "botathonValues.h"

void bluetoothInit(ArduinoLEDMatrix &matrix, BLEService &controlService, BLEUnsignedIntCharacteristic &controlCharacteristic);
void startPairing();
void stopPairing();
bool isPairing();