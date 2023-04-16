#include <AFMotor.h>

AF_DCMotor RightFrontWheel (2);     // M3
AF_DCMotor LeftFrontWheel  (3);     // M4

AF_DCMotor RightBackWheel  (1);     // M2
AF_DCMotor LeftBackWheel   (4);     // M1

int wheelSpeed = 255;
String incomingByte = "";

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  RightFrontWheel.setSpeed(wheelSpeed);
  LeftFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  
  Serial.println("Started");
}

void loop() {
  //if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.readString();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);

    if (incomingByte == "87") {
        Serial.println("W");
        moveForward();
    } else if (incomingByte == "65") {
        Serial.println("A");
        moveSidewaysLeft();
    } else if (incomingByte == "83") {
        Serial.println("S");
        moveBackward();
    } else if (incomingByte == "68") {
        Serial.println("D");
        moveSidewaysRight();
    } else if (incomingByte == "81") {
        Serial.println("Q");
        rotateLeft();
    } else if (incomingByte == "69") {
        Serial.println("E");
        rotateRight();
    } else {
        Serial.println("OFF");
        stopMoving();
    }
  //}
  
  //directionControl();
  //delay(1000);
  //speedControl();
  //delay(1000);
}

void moveForward() {
  LeftFrontWheel.run(FORWARD);
  LeftBackWheel.run(FORWARD);
  RightFrontWheel.run(FORWARD);
  RightBackWheel.run(FORWARD);
}
void moveBackward() {
  LeftFrontWheel.run(BACKWARD);
  LeftBackWheel.run(BACKWARD);
  RightFrontWheel.run(BACKWARD);
  RightBackWheel.run(BACKWARD);
}
void moveSidewaysRight() {
  LeftFrontWheel.run(BACKWARD);
  LeftBackWheel.run(FORWARD);
  
  RightFrontWheel.run(FORWARD);
  RightBackWheel.run(BACKWARD);
}
void moveSidewaysLeft() {
  LeftFrontWheel.run(FORWARD);
  LeftBackWheel.run(BACKWARD);
  
  RightFrontWheel.run(BACKWARD);
  RightBackWheel.run(FORWARD);
}
void rotateLeft() {
  LeftFrontWheel.run(FORWARD);
  LeftBackWheel.run(BACKWARD);
  RightFrontWheel.run(FORWARD);
  RightBackWheel.run(BACKWARD);
}
void rotateRight() {
  LeftFrontWheel.run(BACKWARD);
  LeftBackWheel.run(FORWARD);
  RightFrontWheel.run(BACKWARD);
  RightBackWheel.run(FORWARD);
}
void moveRightForward() {
  LeftFrontWheel.run(FORWARD);
  LeftBackWheel.run(BACKWARD);
  RightFrontWheel.run(FORWARD);
  RightBackWheel.run(BACKWARD);
}
void moveRightBackward() {
  LeftFrontWheel.run(RELEASE);
  LeftBackWheel.run(BACKWARD);
  RightFrontWheel.run(BACKWARD);
  RightBackWheel.run(RELEASE);
}
void moveLeftForward() {
  LeftFrontWheel.run(RELEASE);
  LeftBackWheel.run(FORWARD);
  RightFrontWheel.run(FORWARD);
  RightBackWheel.run(RELEASE);
}
void moveLeftBackward() {
  LeftFrontWheel.run(BACKWARD);
  LeftBackWheel.run(RELEASE);
  RightFrontWheel.run(RELEASE);
  RightBackWheel.run(BACKWARD);
}
void stopMoving() {
  LeftFrontWheel.run(RELEASE);
  LeftBackWheel.run(RELEASE);
  RightFrontWheel.run(RELEASE);
  RightBackWheel.run(RELEASE);
}
