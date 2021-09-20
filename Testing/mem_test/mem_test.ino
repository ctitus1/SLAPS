// Definitions
#define BMP 0 // if 0, code will not execute BMP stuff

// Libraries
#if BMP
#include <Adafruit_BMP280.h>
#endif
#include <Adafruit_INA219.h>
//#include <Adafruit_LSM9DS0.h> // IMU library, currently no used in code
# if BMP
#include <Adafruit_Sensor.h>
#endif
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// Global variables
Adafruit_INA219 ina;
#if BMP
Adafruit_BMP280 bmp;
#endif
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(); // IMU, currently not used
File file;
Servo servo;
const int servo_pin = 6;
const String base_filename = "DATA";
const String ext = ".txt";
const String default_filename = "DEFAULT.txt";
String filename = default_filename; // global variable used by memory functions
enum {BAD, GOOD} state =  GOOD;

// Runs once (uploading code runs code, and opening serial monitor runs code. Having serial monitor open while uploading code
// will make the code execute once (quickly) for uploading, and then again for the serial monitor. This will create two files
// when uploading which is normal. When resetting, only one new file will be created regardless of whether serial monitor
// is open.
void setup() {
  Serial.begin(9600); //Sets the baudrate to 9600

#if BMP
  Serial.println("bmp begin");
  if (!bmp.begin()) // *** PROGRAM FREEZES HERE FOR SOME REASON, BMP SENSOR MIGHT BE BROKEN
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.println(F("BmP check wire"));
    while (1) delay(10);
  }
  Serial.println("bmp done");
#endif

  if (!SD.begin(4)) {
    Serial.println("SDinit failed!");
    while (1);
  }
  Serial.println("mem init");
  mem_init();
  Serial.println("mem write");
  mem_write();
  mem_write();
  mem_write();
  delay(100);
  mem_write();
  delay(100);
  mem_write();
  Serial.println("done");
}

// Loops
void loop() {

}
