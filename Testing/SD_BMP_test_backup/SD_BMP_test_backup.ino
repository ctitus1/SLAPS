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

const String base_filename = "DATA";
const String ext = ".txt";
const String default_filename = "DEFAULT.txt";

String filename = default_filename; // global variable used by memory functions
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
////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  Serial.println("BMP280/SD Card Demo");
  bmp.begin(BMP_address);
  if (SD.begin(SD_pin))
  {
    Serial.println("SD card is present & ready");
  }
  else
  {
    Serial.println("SD card missing or failure");
    while (1); //halt program
  }

mem_init2();

  for (int i = 0; i<2; i++){
  dataStr[0] = 0;
  pressure = bmp.readPressure() / 100; //and conv Pa to hPa
  temperature = bmp.readTemperature();
  altimeter = bmp.readAltitude (QNH); //QNH is local sea lev pressure
  //----------------------- using c-type ---------------------------
  //convert floats to string and assemble c-type char string for writing:
  ltoa( millis(), buffer, 10); //conver long to charStr
  strcat(dataStr, buffer);//add it onto the end
  strcat( dataStr, ", "); //append the delimeter

  //dtostrf(floatVal, minimum width, precision, character array);
  dtostrf(pressure, 5, 1, buffer);  //5 is mininum width, 1 is precision; float value is copied onto buff
  strcat( dataStr, buffer); //append the coverted float
  strcat( dataStr, ", "); //append the delimeter

  dtostrf(temperature, 5, 1, buffer);  //5 is mininum width, 1 is precision; float value is copied onto buff
  strcat( dataStr, buffer); //append the coverted float
  strcat( dataStr, ", "); //append the delimeter

  dtostrf(altimeter, 5, 1, buffer);  //5 is mininum width, 1 is precision; float value is copied onto buff
  strcat( dataStr, buffer); //append the coverted float
  strcat( dataStr, 0); //terminate correctly

  //----- display on local Serial monitor: ------------
  Serial.print(pressure); Serial.print("hPa  ");
  Serial.print(temperature);
  Serial.write(0xC2);  //send degree symbol
  Serial.write(0xB0);  //send degree symbol
  Serial.print("C   ");
  Serial.print(altimeter); Serial.println("m");
  //---------------------------------------------
  // open the file. note that only one file can be open at a time,
  file = SD.open("csv.txt", FILE_WRITE);
  if (file)
  {
    Serial.println("Writing to csv.txt");
    file.println(dataStr);
    file.close();
  }
  else
  {
    Serial.println("error opening csv.txt");
  }
  delay(1000);
  }

}// end setup()

////////////////////////////////////////////////////////////
void loop(void)
{
  
} //end main
///////////////////////////////////////////////

void file_num(int num) {

  // calculates length of global strings
  int base_filename_length = base_filename.length();
  int ext_length = ext.length();
  int filename_length = base_filename_length + 3 + ext_length; // will cause a seg fault if n has more digits than 3

  // allocates space for character buffers (the +1s are for the null character at the end of strings)
  char base_filename_buf[base_filename_length + 1];
  char ext_buf[ext_length + 1];
  char filename_buf[filename_length + 1];

  // check to see if num is too big
  if (num > 999) {
    filename = default_filename;
    Serial.print("ERROR: num > 999, edit filename_length to prevent seg fault from filename_buf being too small");
    return;
  }

  // stores strings base_filename and ext in their respective char[] buffers
  base_filename.toCharArray(base_filename_buf, base_filename_length + 1);
  ext.toCharArray(ext_buf, ext_length + 1);

  // stores formatted char[] in filename_buf
  sprintf(filename_buf, "%s%03i%s", base_filename_buf, num, ext_buf);

  // converts char[] stored in filename_buf to a string and stores it in filename
  filename = String(filename_buf);
}

void mem_init2() {
  //write csv headers to file:
  String headers = "Time,Pressure,Temperature,Altitude"; // string length cannot be more than 34 characters
  filename = "csv.txt";
  
  file = SD.open(filename, FILE_WRITE);
  if (file) // it opened OK
  {
    Serial.print("Writing headers to ");Serial.println(filename);
    file.println(headers);
    file.close();
    Serial.println("Headers written");
  }
  else
    Serial.print("Error opening ");Serial.println(filename);
}

//void mem_init() {
//
//  // column headers for CSV
//  const int num_headers = 6;
//  String col_headers[num_headers] = {"state","time (ms)","pressure (Pa)","temp (C)",
//                                     "altitude (m)","solar_voltage (V)","servo_angle (deg)"};
//
//  // increments ### until its's a unique file
//  int num = 0; // num refers to ### in description, ### is just to show that it's padded by 3 zeroes
//  do {
//    file_num(num++);
//    Serial.println(filename);
//  } while (SD.exists(filename));
//
//  // opens file to write
//  file = SD.open(filename, FILE_WRITE);
//  if (file) { // file opened successfully
//    // write col headers
//    for(int i = 0; i < num_headers; i++) {
//    file.print(col_headers[i]);
//    file.println();
//    }
//  } else { // file opened unsuccessfully
//    Serial.print("error opening "); Serial.print(filename);
//    Serial.println();
//  }
//
//  // closes and saves file
//  file.close();
//}

void mem_write() {

  // declares data variables
  unsigned long current_time;
  float pressure, temp, altitude, solar_voltage;
  int  servo_angle;
  file = SD.open(filename, FILE_WRITE);
  if (file) { // file opened successfully

    // loads data into appropriate variables
    current_time = millis();
    pressure = bmp.readPressure();
    temp = bmp.readTemperature();
    altitude = bmp.readAltitude();
    solar_voltage = ina.getBusVoltage_V();
    servo_angle = servo.read();
    Serial.println("writing");
    // writes data to memory
    file.print(state); file.print(",");
    file.print(current_time); file.print(",");
    file.print(pressure); file.print(",");
    file.print(temp); file.print(",");
    file.print(altitude); file.print(",");
    file.print(solar_voltage); file.print(",");
    file.print(servo_angle); file.println();
  } else { // file opened unsuccessfully
    Serial.print("error opening "); Serial.print(filename);
    Serial.println();
  }

  // closes and saves file
  file.close();
}
