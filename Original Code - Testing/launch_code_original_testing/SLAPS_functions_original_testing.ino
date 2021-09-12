/*
 * Non essential functions for tesing are replaced with prints indicating that the function is executing. The fuctions are currently
 * modified to test the [data writing] aspect of the code.
 */

#define SAVE_TO_MEM 0

void superwrite(int s, float alti){ // *** what is s?
  //current sensor
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = ina219.getBusVoltage_V();
  String to_write = ""; // *** added this so the same things will be written and printed
  //pressure sensor
  sensors_event_t pressure_event;
  bmp_pressure->getEvent(&pressure_event);
  sensors_event_t temp_event;
  bmp_temp->getEvent(&temp_event);
  
  if ((s==1) || (s==3)){
    current_mA=0;
    power_mW=0;
    loadvoltage=0;
  }

  to_write = String(millis())+","+String(loadvoltage)+","+String(current_mA)+","+String(power_mW)
              +","+String(pressure_event.pressure)+","+String(alti)+","+String(temp_event.temperature);

  if (SAVE_TO_MEM){
    DataFile.print(to_write);
    DataFile.println();
  }
  Serial.print(to_write);
  Serial.println();
}


void servoopen(void){
  int open_delay_ms = 20110;
  Serial.print("servoopen executing for ");Serial.print(open_delay_ms);Serial.print(" ms...");
  Serial.println();
  delay(open_delay_ms);
  Serial.print("servoopen done executing.");
  Serial.println();
}


void servoclose(void){
  int close_delay_ms = 20110;
  Serial.print("servoclose executing for ");Serial.print(close_delay_ms);Serial.print(" ms...");
  Serial.println();
  delay(close_delay_ms);
  Serial.print("servoclose done executing.");
  Serial.println();
}
