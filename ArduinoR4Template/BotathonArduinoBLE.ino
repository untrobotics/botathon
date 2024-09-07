
#include "bluetooth.h"

ArduinoLEDMatrix matrix;

BLEService controlService(SERVICE_ID);
BLEUnsignedIntCharacteristic controlCharacteristic(CHARACTERISTIC_ID, BLERead | BLEWrite);
// Motor A connections
int enA = 13;  // 9
int in1 = 12;  // 8
int in2 = 11;  // 7
// Motor B connections
int enB = 8;   // 3
int in3 = 9;   // 5
int in4 = 10;  // 4

// Motor C connections
int enC = 7;
int in5 = 5;
int in6 = 6;

// Motor D connections
int enD = 2;
int in7 = 3;
int in8 = 4;

void setup() {
  delay(2000);
  // start serial
  Serial.begin(BAUD_RATE);
  while (!Serial);
  //start LED matrix
  matrix.begin();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(40);
  matrix.textFont(Font_5x7);

  motorSetUp();

  #ifdef DEBUG_MATRIX
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println("   Starting BLE module   ");
    matrix.endText(SCROLL_LEFT);
  #endif

  bluetoothInit(matrix, controlService, controlCharacteristic);
  startPairing();
  matrix.println(TEAM_NO);
  matrix.endText();
  controlCharacteristic.writeValue(0);

}



unsigned int previousValue = 0;
void loop() {
  BLEDevice central = BLE.central();
  if(isPairing()){
    if(central.connected()){
      stopPairing();
    } else{
      // return;
    }
  } else if(!central.connected()){
    startPairing();
    return;
  }
  if(Serial.available()){
    Serial.read();
    char buffer[50];
    sprintf(buffer, "Current value: %u", controlCharacteristic.value());
    Serial.println(buffer);
  }
  if(controlCharacteristic.written()){
    unsigned int currentValue = controlCharacteristic.value();
    if(previousValue == currentValue) {
      Serial.println("No change");
      return;}
    Serial.println(currentValue);

    if(buttonPressed(currentValue,(Inputs) 1)){
      left_wheels_forwards();
} else{
      stop_left_wheels();
    }
    if(buttonPressed(currentValue, (Inputs) 2)){
      right_wheels_forwards();
    } else{
      stop_right_wheels();
    }

    previousValue = currentValue;
  }
}

void motorSetUp() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(enD, OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);

  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);

  // set all motors to run at maximum speed, when spinning
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  analogWrite(enC, 255);
  analogWrite(enD, 255);

  Serial.println("Motors have been initialised");
}

void left_wheels_forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
}
void right_wheels_forwards() {
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}
void left_wheels_backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
}
void right_wheels_backwards() {
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void stop_left_wheels() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
}

void stop_right_wheels() {
  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void stop_all_wheels() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  digitalWrite(in8, LOW);
  digitalWrite(in7, LOW);
}