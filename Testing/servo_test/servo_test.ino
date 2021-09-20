// Definitions

// Libraries
//#include <Wire.h>
#include <Servo.h>
//#include <SPI.h>

Servo servo;
const int servo_pin = 6;

void setup() {
  Serial.begin(9600); //Sets the baudrate to 9600

  if (!SD.begin(4)) {
    Serial.println("SDinit failed!");
    while (1);
  }


  
}

// Loops
void loop() {

}
