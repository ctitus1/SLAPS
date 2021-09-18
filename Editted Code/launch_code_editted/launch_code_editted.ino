// Libraries
#include <Adafruit_BMP280.h>
#include <Adafruit_INA219.h>
//#include <Adafruit_LSM9DS0.h> // IMU library, currently no used in code
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// Definitions
#define OUTPUT Serial // Serial or file

// Global variables
Adafruit_INA219 ina;
Adafruit_BMP280 bmp;
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(); // IMU, currently not used
File file;
Servo servo;
const int servo_pin = 6;
const String base_filename = "DATA";
const String ext = ".txt";
const String default_filename = "DEFAULT.txt";
String filename = default_filename; // global variable used by memory functions
enum {BAD, GOOD} state =  GOOD;

// Runs once
void setup() {
  Serial.begin(9600); //Sets the baudrate to 9600
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.println(F("BmP check wire"));
    while (1) delay(10);
  }

  if (! ina.begin()) {             //detects if there is a problem with the ina219
    Serial.println("Can't find INA219 ");
    while (1) {
      delay(10);
    }
  }

  file = SD.open("test.txt", FILE_WRITE);
  delay(10);


  if (!SD.begin(4)) {
    Serial.println("SDinit failed!");
    while (1);
  }

  mem_init();
  mem_write();
  delay(500);
  mem_write();
  delay(500);
  mem_write();
  delay(500);
  mem_write();
  delay(500);
  mem_write();
}

// Loops
void loop() {

}
