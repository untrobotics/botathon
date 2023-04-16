// Motor A connections
int in1 = 2; //8
int in2 = 3; //7
// Motor B connections
int in3 = 4; //5
int in4 = 5; //4

// Motor C connections
int in5 = 8; //8
int in6 = 9; //7
// Motor D connections
int in7 = 10; //5
int in8 = 11; //4

String incomingByte = "";

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);

  // Set all the motor control pins to outputs
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

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
        forwards();
    } else if (incomingByte == "65") {
        Serial.println("A");
        left();
    } else if (incomingByte == "83") {
        Serial.println("S");
        backwards();
    } else if (incomingByte == "68") {
        Serial.println("D");
        right();
    } else if (incomingByte == "81") {
        Serial.println("Q");
        rotateCounterClockWise();
    } else if (incomingByte == "69") {
        Serial.println("E");
        rotateClockWise();
    } else {
        Serial.println("OFF");
        stop();
    }
  //}
  
  //directionControl();
  //delay(1000);
  //speedControl();
  //delay(1000);
}

void forwards() {
  digitalWrite(in1, LOW); // right front
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW); // right rear
  digitalWrite(in4, HIGH);
  
  digitalWrite(in5, LOW); // left front
  digitalWrite(in6, HIGH);
  
  digitalWrite(in7, LOW); // left rear
  digitalWrite(in8, HIGH);
}
void backwards() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
}
void left() {
  digitalWrite(in1, LOW); // right front
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH); // right rear
  digitalWrite(in4, LOW);
  
  digitalWrite(in5, HIGH); // left front
  digitalWrite(in6, LOW);
  
  digitalWrite(in7, LOW); // left rear
  digitalWrite(in8, HIGH);
}
void right() {
  digitalWrite(in1, HIGH); // right front
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW); // right rear
  digitalWrite(in4, HIGH);
  
  digitalWrite(in5, LOW); // left front
  digitalWrite(in6, HIGH);
  
  digitalWrite(in7, HIGH); // left rear
  digitalWrite(in8, LOW);
}
void rotateClockWise() {
  digitalWrite(in1, HIGH); // right front
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH); // right rear
  digitalWrite(in4, LOW);
  
  digitalWrite(in5, LOW); // left front
  digitalWrite(in6, HIGH);
  
  digitalWrite(in7, LOW); // left rear
  digitalWrite(in8, HIGH);
}
void rotateCounterClockWise() {
  digitalWrite(in1, LOW); // right front
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW); // right rear
  digitalWrite(in4, HIGH);
  
  digitalWrite(in5, HIGH); // left front
  digitalWrite(in6, LOW);
  
  digitalWrite(in7, HIGH); // left rear
  digitalWrite(in8, LOW);
}
void stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);
}
