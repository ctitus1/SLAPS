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
#define LOCAL_P_MBAR 1022.67 // change to current sea level barrometric pressure (https://www.wunderground.com)
#define FILENAME "test.csv" // Serialname of where data is to be written
#define OPEN_ALTITUDE 20000 // altitude above which the doors will open in meters
#define OPEN_TIME 10000 // time after which the door will open in ms regardless of altitude (need to implement regardless of altitude component)
#define CLOSE_TIME 20000 // time after which the door will close in ms regardless of altitude (need to implement regardless of altitude component)
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
float altitude;

// ******** NEED TO ENSURE STATES ARE WORKING CORRECTLY, THEY ARE NOT RN ******** //
void setup() {
  Serial.begin(9600);
  Serial.println("Start...");

  sensor_init(); // initiates all sensors
  mem_init(); // creates the data file where data will be stored
  mem_write(); // takes an initial data point as soon as memory is set up
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_ANGLE);
  state = servo_closing;
  mem_write(); // takes an initial data point as soon as memory is set up
  delay(100);
  servo.detach();
  state = servo_close;
  mem_write(); // takes an initial data point as soon as memory is set up
  for (int i = 0; i < 3; i++) {
    mem_write();
    delay(100);
  }
  open_door();
  for (int i = 0; i < 3; i++) {
    mem_write();
    delay(100);
  }
  close_door();
  for (int i = 0; i < 3; i++) {
    mem_write();
    delay(100);
  }

  Serial.println("Done!");

}

// ******** NEED TO ENSURE STATES ARE WORKING CORRECTLY, THEY ARE NOT RN ******** //
void loop() {



}

void close_door() {

  servo.attach(SERVO_PIN);
  state = servo_closing;
  for (int angle = OPEN_ANGLE; angle >= CLOSE_ANGLE; angle -= DEG_PER_STEP) {

    servo.write(angle); // tell servo to go to specified angle
    mem_write(); // takes an initial data point as soon as memory is set up
    delay(DEG_PER_STEP * DELAY_PER_DEG); // waits for the servo to reach the position

  }

#if DETATCH_AFTER_MOVING
  servo.detach();
#endif
  state = servo_close;
  mem_write(); // takes an initial data point as soon as memory is set up

}

void open_door() {

  servo.attach(SERVO_PIN);
  state = servo_opening;
  for (int angle = CLOSE_ANGLE; angle <= OPEN_ANGLE; angle += DEG_PER_STEP) {

    servo.write(angle); // tell servo to go to specified angle
    mem_write(); // takes an initial data point as soon as memory is set up
    delay(DEG_PER_STEP * DELAY_PER_DEG); // waits for the servo to reach the position

  }

#if DETATCH_AFTER_MOVING
  servo.detach();
#endif
  state = servo_open;
  mem_write(); // takes an initial data point as soon as memory is set up

}

void sensor_init() {
  ///////////////////////////
  // Initialize BMP Sensor //
  ///////////////////////////
  if (!bmp.begin(BMP_ADDRESS)) {

    err = bmp_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }
  ///////////////////////////

  ////////////////////////
  // Initialize SD card //
  ////////////////////////
  if (!SD.begin(SD_PIN)) {

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }
  ////////////////////////

  ////////////////////
  // Initialize LSM //
  ////////////////////
  if (!lsm.begin()) {

    err = lsm_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  } else {

    // set range/sensitivity of IMU
    lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);   // 2G, 4G, 6G, 8G, 16G
    lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);     // 2GAUSS, 4GAUSS, 8GAUSS
    lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS); // 245DPS, 500DPS, 2000DPS

  }
  ////////////////////

}

void mem_init() {

  file = SD.open(FILENAME, FILE_WRITE); // creates and names file

  if (file) {

#if HEADERS
#if FULL_HEADERS
//  file.println("time,err,state,pressure,temp,altitude,sh_voltage,bu_voltage,current,power,servo_angle,A_x,A_y,A_z,G_x,G_y,G_z"); // full headers
    file.println("time,err,state,pressure,temp,altitude,sh_voltage,bu_voltage,current,power,servo_angle,A_x,A_y,A_z,G_x,G_y,G_z"); // full headers
#else
//  file.println("ti,e,st,pr,te,a,Vs,Vb,c,po,Sa,Ax,Ay,Az,Gx,Gy,Gz"); // short headers
    file.println("ti,e,st,pr,te,a,Vs,Vb,c,po,Sa,Ax,Ay,Az,Gx,Gy,Gz"); // short headers
#endif

#if UNITS
//  file.println("ms,,,Pa,C,m,mV,V,mA,mW,deg,g,g,g,dps,dps,dps"); // units
    file.println("ms,,,Pa,C,m,mV,V,mA,mW,deg,g,g,g,dps,dps,dps"); // units
#endif
#endif

    file.close(); // closes file

  } else {

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }

}

void mem_write() {

  file = SD.open(FILENAME, FILE_WRITE);

  if (file) { // writes data to file

    lsm.read();

    file.print(millis()); file.print(",");                        // time
    file.print(err); file.print(",");                             // err
    file.print(state); file.print(",");                           // state
    file.print(bmp.readPressure()); file.print(",");              // pressure
    file.print(bmp.readTemperature()); file.print(",");           // temp
    file.print(bmp.readAltitude(LOCAL_P_MBAR)); file.print(",");  // altitude
    file.print(ina.getShuntVoltage_mV()); file.print(",");        // sh_voltage
    file.print(ina.getBusVoltage_V()); file.print(",");           // bu_voltage
    file.print(ina.getCurrent_mA()); file.print(",");             // current
    file.print(ina.getPower_mW()); file.print(",");               // power
    file.print(servo.read()); file.print(",");                    // servo_angle
    file.print(lsm.accelData.x); file.print(",");                 // A_x
    file.print(lsm.accelData.y); file.print(",");                 // A_y
    file.print(lsm.accelData.z); file.print(",");                 // A_z
    file.print(lsm.gyroData.x); file.print(",");                  // G_x
    file.print(lsm.gyroData.y); file.print(",");                  // G_y
    file.print(lsm.gyroData.z); file.println();                   // G_z
    file.close();

  } else {

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }
}
