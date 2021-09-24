// Main code

// Definitions
#define HALT_ON_FAILURE 0 // if 1, program will halt on any failure, if 0, it will continue despite errors if it is able to
#define DETACH_AFTER_MOVING 1 // if 1, servos will detach after moving, if 0, they will not
#define SD_PIN 4 // pin the SD card is connected to (chip select aka CS pin)
#define SERVO_PIN 6 // pin the servo is connected to
#define OPEN_ANGLE 110 // angle for servo to be when open in deg (used to be 50, my servo can't go that far)
#define CLOSE_ANGLE 0 // angle for servo to be at when closed in deg (used to be 175, my servo can't go that far)
#define DELAY_PER_DEG 11 // delay per degree of movement of servo in ms
#define DEG_PER_STEP 5 // degrees moved per step of servo
#define LOOP_DELAY 100 // time to wait between loops in ms
#define BMP_ADDRESS 0x77 // address for BMP sensor, can be 0x76 or 0x77, but hardware changes are needed (see documentation)
#define LOCAL_P_MBAR 1010.84 // change to current sea level barrometric pressure (https://www.wunderground.com)
#define FILENAME "test.csv" // Serialname of where data is to be written
#define OPEN_PRESSURE 85000 // pressure below which the doors will open in Pa (~1500m or 5000ft)
#define OPEN_PRESSURE_BUFFER 1000 // 
//#define OPEN_TIME 10000 // time after which the door will open in ms regardless of altitude (need to implement regardless of altitude component)
//#define CLOSE_TIME 20000 // time after which the door will close in ms regardless of altitude (need to implement regardless of altitude component)
#define FULL_HEADERS 0 // 0 for abbreviated headers in data Serial, 1 for full ones
#define UNITS 0 // 1 for units in data Serial, 0 for none
#define HEADERS 1 // 0 headers and units by those specified above, 1 for neither regardless of above specification

// Libraries
#include <Adafruit_BMP280.h>
#include <Adafruit_INA219.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h> // IMU library, currently not used in code
#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// Objects
Adafruit_BMP280 bmp;
Adafruit_INA219 ina;
Adafruit_LSM9DS0 lsm;
File file;
Servo servo;

// Global Variables
unsigned long current_time;
enum {default_state, servo_close, servo_opening, servo_open, servo_closing, servo_detached} state =  default_state;
enum {no_err, bmp_err, sd_err, mem_init_err, mem_write_err, lsm_err} err = no_err;
float altitude = 0;

void setup() {

  // initializes everything
  Serial.begin(9600);
  Serial.println("sen_init");
  sensor_init();
  Serial.println("mem_init");
  mem_init();
  Serial.println("init done");

  mem_write(); // writes to memory ASAP

  // ensures door is closed
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_ANGLE);
  state = servo_closing;
  mem_write();
  delay(100);
  servo.detach();
  state = servo_close;

  // while below target altitude+buffer, stay in state a (idk what to name it)
//  while (millis() < 5000) { // use for bench test
  while (bmp.readPressure() > (OPEN_PRESSURE - OPEN_PRESSURE_BUFFER / 2)) { // use for actual test
    Serial.println("a");
    Serial.println(bmp.readPressure());
    mem_write();
    delay(100);
  }

  // once altitude is above target altitude+buffer, open door and go to state b
  open_door();

  // while altitude is above target altitude-buffer, stay in state b
//  while (millis() < 10000) { // use for bench test
  while (bmp.readPressure() < (OPEN_PRESSURE + OPEN_PRESSURE_BUFFER / 2)) { // use for actual test
    Serial.println("b");
    Serial.println(bmp.readPressure());
    mem_write();
    delay(100);
  }

  // once altitude is below target altitude-buffer, close door and transition to state c
  close_door();

  // just take data
  while (1) {
    Serial.println("c");
    Serial.println(bmp.readPressure());
    mem_write();
    delay(100);
  }

}

void loop() {

}
