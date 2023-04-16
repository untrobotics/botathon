#include <ArduinoJson.h>
#include <AFMotor.h>
#define FRONT_L 2
#define FRONT_R 1
#define BACK_L 3
#define BACK_R 4
AF_DCMotor FL_motor(FRONT_L,MOTOR12_1KHZ);
AF_DCMotor FR_motor(FRONT_R,MOTOR12_1KHZ);
AF_DCMotor BL_motor(BACK_L,MOTOR12_1KHZ);
AF_DCMotor BR_motor(BACK_R,MOTOR12_1KHZ);

StaticJsonDocument<200> doc;

String incomingByte = "";

void setup() {  
  // Serial.println("Started");
  Serial.begin(115200);
  FL_motor.setSpeed(200);
  FR_motor.setSpeed(200);
  BL_motor.setSpeed(200);
  BR_motor.setSpeed(200);
}

void move_motor(AF_DCMotor motor, int num, bool forward)
{
    if(forward)
      if(num <= 2)
        motor.run(BACKWARD);
      else
        motor.run(FORWARD);
    else
      if(num<=2)     
        motor.run(FORWARD);
        else
        motor.run(BACKWARD);        
}
void forward()
{
move_motor(FL_motor,FRONT_L,true)  ;
move_motor(FR_motor,FRONT_R,true)  ;
move_motor(BL_motor,BACK_L,true)  ;
move_motor(BR_motor,BACK_R,true)  ;
}
void backward()
{
  move_motor(FL_motor,FRONT_L,false)  ;
move_motor(FR_motor,FRONT_R,false)  ;
move_motor(BL_motor,BACK_L,false)  ;
move_motor(BR_motor,BACK_R,false)  ;
}
void stop()
{
  FL_motor.run(RELEASE);
  FR_motor.run(RELEASE);
  BL_motor.run(RELEASE);
  BR_motor.run(RELEASE);
}
bool A_BUTTON_PRESSED = false;

void loop() {
  if (Serial.available() > 0) {
   //// read the incoming byte:
        Serial.println("Reading input");
        auto input = Serial.read();
        if(input == 'f')
        {
          forward();
        }
        else if(input == 'b')
        {
              backward();
        }
        else if(input == 's')
        stop();
       
  }
    // incomingByte = Serial.readStringUntil('\n');
//     Serial.println("Motor 1 forwards");
//     motor1.run(FORWARD);
//     delay(1000);
//     Serial.println("Motor 1 backwards");
//     motor1.run(BACKWARD);
//     delay(1000);
//     motor1.run(RELEASE);
//     Serial.println("Motor 2 forwards");
//     motor2.run(FORWARD);
//     delay(1000);
//     Serial.println("Motor 2 backwards");
//     motor2.run(BACKWARD);
//     delay(1000);
//     motor2.run(RELEASE);
    
// Serial.println("Motor 3 forwards");
//     motor3.run(FORWARD);
//     delay(1000);
//     Serial.println("Motor3 backwards");
//     motor3.run(BACKWARD);
//     delay(1000);
//     motor3.run(RELEASE);
// Serial.println("Motor 4 forwards");
//     motor4.run(FORWARD);
//     delay(1000);
//     Serial.println("Motor4 backwards");
//     motor4.run(BACKWARD);
//     delay(1000);
//     motor4.run(RELEASE);

// Serial.print("INput was ");
    
    // // say what you got:
    // Serial.print("I received: ");
    // Serial.println(incomingByte);

    // DeserializationError error = deserializeJson(doc, incomingByte);

    // if (error) {
    //   Serial.print(F("deserializeJson() failed: "));
    //   Serial.println(error.f_str());
    //   return;
    // }

    // String key = doc["input"]["key"];
    // float value = doc["input"]["value"].as<float>();
    
    // Serial.print("KEY:");
    // Serial.print(key);
    // Serial.print(", VAL:");
    // Serial.println(value);

    // if (key == "A_BUTTON") {
    //   if (value == true) {
    //     A_BUTTON_PRESSED = !A_BUTTON_PRESSED;
    //     Serial.print("Direction switched:");
    //     Serial.println(A_BUTTON_PRESSED);
    //   }
    // }

    // if (key == "RIGHT_TRIGGER") {
    //   if (value > -1) {
    //     if (A_BUTTON_PRESSED) {
    //       right_forwards();
    //     } else {
    //       right_backwards();
    //     }
    //   } else {
    //     stop_right();
    //   }
    // }

    // if (key == "LEFT_TRIGGER") {
    //   if (value > -1) {
    //     if (A_BUTTON_PRESSED) {
    //       left_forwards(); 
    //     } else {
    //       left_backwards();
    //     }
    //   } else {
    //     stop_left();
    //   }
    // }
  //}
  
  //directionControl();
  //delay(1000);
  //speedControl();
  //delay(1000);
}