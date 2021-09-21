// Definitions
#define HALT_ON_FAILURE 0 // if 1, program will halt on any failure, if 0, it will continue despite errors if it is able to
#define DETACH_AFTER_MOVING 1 // if 1, servos will detach after moving, if 0, they will not
#define SD_PIN 4 // pin the SD card is connected to (chip select aka CS pin)
#define SERVO_PIN 6 // pin the servo is connected to
#define OPEN_ANGLE 50 // angle for servo to be when open in deg
#define CLOSE_ANGLE 175 // angle for servo to be at when closed in deg
#define SERVO_DELAY 1000 // time to wait for servo to move in ms
#define LOOP_DELAY 100 // time to wait between loops in ms
#define BMP_ADDRESS 0x77 // address for BMP sensor, can be 0x76 or 0x77, but hardware changes are needed (see documentation)
#define LOCAL_P_MBAR 1022.67 // change to current sea level barrometric pressure (https://www.wunderground.com)
#define FILENAME "test.txt" // filename of where data is to be written
#define OPEN_ALTITUDE 20000 // altitude above which the doors will open
#define OPEN_TIME 10000 // time after which the door will open in ms regardless of altitude (need to implement regardless of altitude component)
#define CLOSE_TIME 20000 // time after which the door will close in ms regardless of altitude (need to implement regardless of altitude component)
#define ERR_LED 8 // pin which serves as output to error LED

// Libraries
#include <Adafruit_BMP280.h>
#include <Adafruit_INA219.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_LSM9DS0.h> // IMU library, currently not used in code
#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// Objects
Adafruit_BMP280 bmp;
Adafruit_INA219 ina;
File file;
Servo servo;

// Global Variables
unsigned long current_time;
enum {default_state, servo_close, servo_opening, servo_open, servo_closing} state =  default_state;
enum {no_err, bmp_err, sd_err, mem_init_err, mem_write_err} err = no_err;
float altitude;

void setup() {
  pinMode(ERR_LED,OUTPUT);
  digitalWrite(ERR_LED,err);
  
  sensor_init(); // initiates all sensors
  digitalWrite(ERR_LED,err);
  
  mem_init(); // creates the data file where data will be stored
  digitalWrite(ERR_LED,err);
  mem_write(); // takes an initial data point as soon as memory is set up
  digitalWrite(ERR_LED,err);
  
  close_door(0); // closes door ignoring redundancy checks implemented in close_door()
  digitalWrite(ERR_LED,err);
  mem_write();
  digitalWrite(ERR_LED,err);
}

void loop() {
  altitude = bmp.readAltitude(LOCAL_P_MBAR);
  current_time = millis();
  if (altitude > OPEN_ALTITUDE || (current_time > OPEN_TIME && current_time < CLOSE_TIME)) {
    open_door(1);
    digitalWrite(ERR_LED,err);
  } else {
    close_door(1);
    digitalWrite(ERR_LED,err);
  }
  mem_write();
  digitalWrite(ERR_LED,err);
  delay(LOOP_DELAY);
}


void close_door(int use_redunancy_checks) {
  if (use_redunancy_checks && ( state == servo_close || state == servo_closing )) {
    return;
  } else {
    state = servo_closing;
    servo.attach(SERVO_PIN);
    servo.write(CLOSE_ANGLE);
    mem_write(); // takes data point just before delay for closing servo
    delay(SERVO_DELAY); // should be replaced with a non-blocking wait
    state = servo_close;
    #if DETATCH_AFTER_MOVING
      servo.detach();
    #endif
  }
}

void open_door(int use_redunancy_checks) {
  if (use_redunancy_checks && ( state == servo_open || state == servo_opening )) {
    return;
  } else {
    state = servo_opening;
    servo.attach(SERVO_PIN);
    servo.write(OPEN_ANGLE);
    mem_write(); // takes data point just before delay for opening servo
    delay(SERVO_DELAY); // should be replaced with a non-blocking wait
    state = servo_open;
    #if DETATCH_AFTER_MOVING
      servo.detach();
    #endif
  }
}

void sensor_init() {
  /////////////////////////////////////
  // Initialize serial communication //
  /////////////////////////////////////
  Serial.begin(9600);
  /////////////////////////////////////

  ///////////////////////////
  // Initialize BMP Sensor //
  ///////////////////////////
  Serial.print("Initializing BMP sensor at address "); Serial.print(BMP_ADDRESS); Serial.print("... ");
  if (bmp.begin(BMP_ADDRESS)) {
    Serial.println("success!");
  } else {
    Serial.println("initialization failed.");
    #if HALT_ON_FAILURE // Might want to condense this into a function or macro, works fine for now
      Serial.println("Program halted.");
      while (1);
    #else
      Serial.println("Program continuing.");
      err = bmp_err;
    #endif
  }
  ///////////////////////////

  ////////////////////////
  // Initialize SD card //
  ////////////////////////
  Serial.print("Initializing SD card at pin "); Serial.print(SD_PIN); Serial.print("... ");
  if (!SD.begin(SD_PIN)) {
    Serial.println("initialization failed.");
    #if HALT_ON_FAILURE // Might want to condense this into a function or macro, works fine for now
      Serial.println("Program halted.");
      while (1);
    #else
      Serial.println("Program continuing.");
      err = sd_err;
    #endif
  }
  Serial.println("success!");
  ////////////////////////
}

void mem_init() {
  /////////////////////////////////
  // Initialize file to write to //
  /////////////////////////////////
  String header = "time (ms),err,state,pressure (Pa),temp (C),altitude (m),solar_voltage (V),servo_angle (deg)";

  Serial.print("Opening "); Serial.print(FILENAME); Serial.print("... ");
  file = SD.open(FILENAME, FILE_WRITE);

  if (file) {
    Serial.print("success!");
    Serial.println();

    Serial.print("Writing headers to "); Serial.print(FILENAME); Serial.print("... ");
    file.println(header);
    file.close();

    Serial.println("success!");
  } else {
    Serial.print("opening "); Serial.print(FILENAME); Serial.println(" in mem_init() failed");
    #if HALT_ON_FAILURE // Might want to condense this into a function or macro, works fine for now
      Serial.println("Program halted.");
      while (1);
    #else
      Serial.println("Program continuing.");
      err = mem_init_err;
    #endif
  }
  /////////////////////////////////
}

void mem_write() {
  Serial.print("Opening "); Serial.print(FILENAME); Serial.print("... ");
  file = SD.open(FILENAME, FILE_WRITE);
  if (file) {
    Serial.print("success!");
    Serial.println();

    float pressure, temp, solar_voltage;
    int servo_angle;

    Serial.print("Measuring data... ");
    current_time = millis();
    pressure = bmp.readPressure();
    temp = bmp.readTemperature();
    altitude = bmp.readAltitude(LOCAL_P_MBAR);
    solar_voltage = ina.getBusVoltage_V();
    servo_angle = servo.read();
    Serial.print("success!");
    Serial.println();

    Serial.print("Writing to "); Serial.print(FILENAME); Serial.print("... ");
    file.print(current_time); file.print(",");
    file.print(err); file.print(",");
    file.print(state); file.print(",");
    file.print(pressure); file.print(",");
    file.print(temp); file.print(",");
    file.print(altitude); file.print(",");
    file.print(solar_voltage); file.print(",");
    file.print(servo_angle); file.println();
    file.close();
    Serial.print("success! File closed.");
    Serial.println();

  } else {
    Serial.print("opening "); Serial.print(FILENAME); Serial.print(" in mem_write() failed");
    Serial.println();
    #if HALT_ON_FAILURE // Might want to condense this into a function or macro, works fine for now
      Serial.println("Program halted.");
      while (1);
    #else
      Serial.println("Program continuing.");
      err = mem_write_err;
    #endif
  }
}
