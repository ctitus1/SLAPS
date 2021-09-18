// Definitions
#define BMP 0

// Libraries
//#include <Adafruit_BMP280.h>
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

// Runs once
void setup() {
  Serial.begin(9600); //Sets the baudrate to 9600
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#if BMP
  Serial.println("bmp begin");
  if (!bmp.begin()) // *** PROGRAM FREEZES HERE FOR SOME REASON
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
  Serial.println("init");
  mem_init();
  Serial.println("write");
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
