/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/
#define FILENAME "test.txt"

#include <Adafruit_BMP280.h>
#include <Adafruit_INA219.h>
//#include <Adafruit_LSM9DS0.h> // IMU library, currently no used in code
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
Adafruit_BMP280 bmp;
Adafruit_INA219 ina;

File file;
Servo servo;
const int SD_pin = 4;
const int servo_pin = 6;
const int BMP_address = 0x77;

enum {BAD, GOOD} state =  GOOD;
float QNH = 1022.67; //Change the "1022.67" to your current sea level barrometric pressure (https://www.wunderground.com)
float pressure;
float temperature;
float altimeter;
char charRead;
char runMode;
byte i = 0;
char dataStr[100] = "";
char buffer[7];

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  bmp.begin(BMP_address);
  pressure = bmp.readPressure();

  Serial.print("Initializing SD card...");
  mem_init();

  Serial.println(myFile);
  myFile.println("test");
  myFile.println(pressure);
  myFile.close();
}

void loop() {
  // nothing happens after setup
}

void mem_init() {
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.


  String headers[7] = {"state", "time (ms)", "pressure (Pa)", "temp (C)", "altitude (m)", "solar_voltage (V)", "servo_angle"};
  int num_headers = sizeof(headers) / sizeof(String);
  myFile = SD.open(FILENAME, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing headers to "); Serial.print(FILENAME); Serial.println("...");
    for (int i = 0; i < num_headers; i++) {
      myFile.print(headers[i]);
      if (i < num_headers - 1) {
        myFile.print(",");
      }
      myFile.flush();
    }
    myFile.println();
    // close the file:
    myFile.flush();
    Serial.println("writing done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
