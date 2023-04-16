#include <ArduinoJson.h>

StaticJsonDocument<200> doc;

// Motor A connections
int enA = 9; //9
int in1 = 8; //8
int in2 = 7; //7
// Motor B connections
int enB = 3; //3
int in3 = 5; //5
int in4 = 4; //4

String incomingByte = "";

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  Serial.println("Started");
}

bool A_BUTTON_PRESSED = false;

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.readStringUntil('\n');

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);

    DeserializationError error = deserializeJson(doc, incomingByte);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String key = doc["key"];
    float value = doc["value"].as<float>();
    
    Serial.print("KEY:");
    Serial.print(key);
    Serial.print(", VAL:");
    Serial.println(value);

    if (key == "A_BUTTON") {
      if (value == true) {
        A_BUTTON_PRESSED = !A_BUTTON_PRESSED;
        Serial.print("Direction switched:");
        Serial.println(A_BUTTON_PRESSED);
      }
    }

    if (key == "RIGHT_TRIGGER") {
      if (value > -1) {
        if (A_BUTTON_PRESSED) {
          right_forwards();
        } else {
          right_backwards();
        }
      } else {
        stop_right();
      }
    }

    if (key == "LEFT_TRIGGER") {
      if (value > -1) {
        if (A_BUTTON_PRESSED) {
          left_forwards(); 
        } else {
          left_backwards();
        }
      } else {
        stop_left();
      }
    }
  }
  
  //directionControl();
  //delay(1000);
  //speedControl();
  //delay(1000);
}

void forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}
void backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void left_forwards() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}
void right_forwards() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void left_backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}
void right_backwards() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stop_left() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void stop_right() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control spinning direction of motors
void directionControl() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  // Turn on motor A & B
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(2000);
  
  // Now change motor directions
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(2000);
  
  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
  // Turn on motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  // Accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i) {
    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
  }
  
  // Now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
