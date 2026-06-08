
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
  // Creates an object that contains information about the computer that's connected to the Arduino via Bluetooth
  BLEDevice central = BLE.central();

  // if the Arduino is currently pairing, stop pairing if it connected to the computer (via Bluetooth)
  if(isPairing())
  {
    // checks if the computer is connected to the Arduino
    if(central.connected()){
      // stops pairing process
      stopPairing();
    }
    else
    {
      // restart the loop since no computer connection = no controller input
      return;
    }
  }
  // if the Arduino isn't pairing, and there isn't a computer connected to the Arduino, start pairing
  else if(!central.connected())
  {
    startPairing();
    return;
  }

  // Checks if the variable has a new value i.e., it's been updated and changed
  if(controlCharacteristic.written())
  {
    // refer to currentValue for the current controller value
    unsigned int currentValue = controlCharacteristic.value();

    /***INSERT YOUR CONTROLLER CODE BELOW HERE***/

    /** // Below is a usage example
    // checks if button A is pressed
    if(buttonPressed(currentValue,Inputs::AButton))
    {
    // sets the left servos to move forward
      left_wheels_forwards();
    }
    else
    {
    // stops the left wheels
      stop_left_wheels();
    }
    **/

    /***INSERT YOUR CONTROLLER CODE ABOVE HERE***/

    // update previous value to current value
    previousValue = currentValue;
  }
}

// sets up pins to output on the Arduino. Also sets the motor speed to max
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
  // if your L298N has a jumper on the enables, these lines aren't needed
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  analogWrite(enC, 255);
  analogWrite(enD, 255);

  Serial.println("Motors have been initialized");
}

// turns on the left wheels to move forwards
void left_wheels_forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
}

// turns on the right wheels to move forwards
void right_wheels_forwards() {
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}
// turns on the left wheels to move backwards
void left_wheels_backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
}
// turns on the right wheels to move backwards
void right_wheels_backwards() {
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
// turns off the left wheels
void stop_left_wheels() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
}
// turns off the right wheels
void stop_right_wheels() {
  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// turns off all of the wheels
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