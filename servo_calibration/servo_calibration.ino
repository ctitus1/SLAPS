#define SERVO_PIN 6
#define OPEN_ANGLE 110
#define CLOSE_ANGLE 0
#define DELAY_PER_DEG 11
#define DEG_PER_STEP 5

#include <Servo.h>

Servo l_servo;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial monitor initiated");
  servoopen();
  delay(2000);
  servoclose();
}

void loop() {
  
}

void servoopen(void){
  int lang = CLOSE_ANGLE;
  l_servo.attach(6); //Defines left servo to attach at pin 6(can be any pin number)
  l_servo.write(lang); //opens left
  delay(1000);
  Serial.println("Moving servo...");
  for (lang = CLOSE_ANGLE; lang <= OPEN_ANGLE; lang += DEG_PER_STEP) { // goes from -5 degrees to 90 degrees
//    // in steps of 1 degree
    Serial.println(lang);
    l_servo.write(lang);              // tell servo to go to position in variable 'pos'
    delay(DEG_PER_STEP*DELAY_PER_DEG);                       // waits 15ms for the servo to reach the position
  }
  l_servo.detach();

}


void servoclose(void){
  int lang = OPEN_ANGLE;
  l_servo.attach(6); //Defines left servo to attach at pin 9(can be any pin number)
  l_servo.write(lang); //closes left
  delay(1000);
  Serial.println("Moving servo...");
  for (lang = OPEN_ANGLE; lang >= CLOSE_ANGLE; lang -= DEG_PER_STEP) { // goes from 180 degrees to -5 degrees
    l_servo.write(lang);              // tell servo to go to position in variable 'pos'
    Serial.println(lang);
    delay(DEG_PER_STEP*DELAY_PER_DEG);                       // waits 15ms for the servo to reach the position
  }
  l_servo.detach();
}
