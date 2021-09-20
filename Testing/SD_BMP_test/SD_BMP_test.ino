#include <SD.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; 
File myFile;

// change this to match your SD shield or module:
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
//mega pin = 53;
const int chipSelect = 4;
float QNH = 1022.67; //Change the "1022.67" to your current sea level barrometric pressure (https://www.wunderground.com)
const int BMP_address = 0x76;

float pressure;   
float temperature;  
float altimeter; 
char charRead;
char runMode;
byte i=0; //counter
char dataStr[100] = "";
 char buffer[7];
////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  Serial.println("BMP280/SD Card Demo");
  bmp.begin(BMP_address); 
  if (SD.begin(chipSelect))
  {
    Serial.println("SD card is present & ready");
  } 
  else
  {
    Serial.println("SD card missing or failure");
    while(1); //halt program
  }
  //clear out old data file
  if (SD.exists("csv.txt")) 
  {
    Serial.println("Removing simple.txt");
    SD.remove("csv.txt");
    Serial.println("Done");
  } 

  //write csv headers to file:
   myFile = SD.open("csv.txt", FILE_WRITE);  
   if (myFile) // it opened OK
    {
    Serial.println("Writing headers to csv.txt");
    myFile.println("Time,Pressure,Temperature,Altitude");
    myFile.close(); 
    Serial.println("Headers written");
    }
  else 
    Serial.println("Error opening csv.txt");  
  Serial.println("Enter w for write, r for read or s for split csv");  
}// end setup()

////////////////////////////////////////////////////////////
void loop(void) 
{
 dataStr[0] = 0;
 pressure = bmp.readPressure()/100;  //and conv Pa to hPa
 temperature = bmp.readTemperature();
 altimeter = bmp.readAltitude (QNH); //QNH is local sea lev pressure
//----------------------- using c-type ---------------------------
 //convert floats to string and assemble c-type char string for writing:
 ltoa( millis(),buffer,10); //conver long to charStr
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
 //Serial.println(dataStr);
 //---------------------------------------------------  
 //create a loop to read from the keyboard a command character
 //this will be 'r' for read, 'w' for write and 'd' for delete.

  if (Serial.available()) //get command from keyboard:
     {
      charRead = tolower(Serial.read());  //force ucase
      Serial.write(charRead); //write it back to Serial window
      Serial.println();
     }
 
  if(charRead == 'w')  //we are logging
      runMode = 'W';
  if(charRead == 'r')  //we are reading
      runMode = 'R';
  if(charRead == 'd')  //we are deleting
      runMode = 'D';
 //----------------------------------------------------
  if(runMode == 'W') //write to file
  {     
   //----- display on local Serial monitor: ------------
   Serial.print(pressure); Serial.print("hPa  ");
   Serial.print(temperature); 
   Serial.write(0xC2);  //send degree symbol
   Serial.write(0xB0);  //send degree symbol
   Serial.print("C   ");  
   Serial.print(altimeter); Serial.println("m");
   //---------------------------------------------
   // open the file. note that only one file can be open at a time,
    myFile = SD.open("csv.txt", FILE_WRITE);     
    // if the file opened okay, write to it:
    if (myFile) 
    {
      Serial.println("Writing to csv.txt");
      myFile.println(dataStr); 
      myFile.println(dataStr); 
      myFile.close();
    } 
    else 
    {
      Serial.println("error opening csv.txt");
    }
    delay(1000);  
  }
//--------------------------------------------------
    if(runMode == 'R')  //we are reading
   {
    if (!SD.exists("csv.txt")) 
        Serial.println("csv.txt doesn't exist."); 
   Serial.println("Reading from csv.txt");
   myFile = SD.open("csv.txt");
    
   while (myFile.available()) 
  {   
   char inputChar = myFile.read(); // Gets one byte from serial buffer
    if (inputChar == '\n') //end of line (or 10)
    {
      dataStr[i] = 0;  //terminate the string correctly
      Serial.println(dataStr);
      i=0; //reset the counter
    }
    else
    {
      dataStr[i] = inputChar; // Store it
      i++; // Increment where to put next char
      if(i> sizeof(dataStr))  //error checking for overflow
        {
        Serial.println("Incoming string longer than array allows");
        Serial.println(sizeof(dataStr));
        while(1);
        }
    }
  }
}
  //------------------------------------------- 
  if(runMode=='D')
  {
   //delete a file:
   if (SD.exists("csv.txt")) 
      {
      Serial.println("Removing csv.txt");
      SD.remove("csv.txt");
      Serial.println("Done");
     } 
  }
  //-----------------------------------------
   
} //end main
///////////////////////////////////////////////
