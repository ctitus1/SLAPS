

//include all necessary libraries here
#include <Adafruit_BMP280.h>            
#include <Adafruit_INA219.h> 
//#include <Adafruit_LSM9DS0.h>
#include <Wire.h>                  
#include <Adafruit_Sensor.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
//define all necessary objects here
Servo l_servo; //defines the left servo object
Servo r_servo; //defines the right servo object
Adafruit_INA219 ina219; //defines the ina object
Adafruit_BMP280 bmp;
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();
File DataFile;


//define global time which is unsigned long int
unsigned long currentMillis = millis();
unsigned long logtime = 0;
//unsigned long IMUtime = 0;
unsigned long lasttime = 0;

int state=1;
float alt;
const float local_p=1011.6; //adjust to local forecast!!


void setup() {
  Serial.begin(9600); //Sets the baudrate to 9600
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.println(F("BmP check wire"));
    while (1) delay(10);
  }

  if (! ina219.begin()) {             //detects if there is a problem with the ina219
    Serial.println("Can't find INA219 ");
    while (1) { delay(10); }
  }

  DataFile = SD.open("test.txt", FILE_WRITE);
  delay(1);


  if (!SD.begin(4)) {
    Serial.println("SDinit failed!");
    while (1);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
    DataFile = SD.open("data.csv", FILE_WRITE);

}

void loop() {
  currentMillis=millis();
  if (state==1){
    alt=bmp.readAltitude(local_p);
    if (millis()>logtime+1000UL){
      logtime = currentMillis;
      superwrite(state, alt);
    }

    if (alt>=1524){ //can be any height
      state=2; //changes state
      DataFile.println("Change to 2. Timestamp: "+ currentMillis);
      servoopen(); //defines the opening process to get the servo to go to a certain angle
    }
  }
  else if (state==2){
    alt=bmp.readAltitude(local_p); 
    if (currentMillis>logtime+1000UL){
      logtime = currentMillis;
      superwrite(state, alt);
    }

    if (alt>=19812){ //can be any height
      state=3; //changes state
      DataFile.println("Change to 3. Timestamp: "+ currentMillis); 
      servoclose(); //defines the closing process to get the servo to go to a certain angle
    }
  }
  else {
    alt=bmp.readAltitude(local_p); 
    if (currentMillis>logtime+1000UL){
      logtime = currentMillis;
      superwrite(state, alt);
  }
  if (millis()>lasttime+5000UL){
    lasttime=currentMillis;
    DataFile.flush(); //need to talk about how this works
  }
 
  }
}
