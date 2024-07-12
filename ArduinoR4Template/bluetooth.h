#include <ArduinoBLE.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "botathonValues.h"

#define PAIR_INTERVAL 30000   // interval for pairing after button press in ms

void bluetoothInit(ArduinoLEDMatrix &matrix, BLEService &controlService, BLEUnsignedIntCharacteristic &controlCharacteristic);
void startPairing();
void stopPairing();
bool isPairing();