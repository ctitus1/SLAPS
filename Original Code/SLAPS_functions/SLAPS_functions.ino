
void superwrite(int s, float alti){
  //current sensor
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = ina219.getBusVoltage_V();
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
  DataFile.print(String(millis())+",");
  DataFile.print(String(loadvoltage)+","+String(current_mA)+","+String(power_mW)+","+String(pressure_event.pressure)+","+String(alti)+","+String(temp_event.temperature));
  DataFile.println();
}


void servoopen(void){
  int lang = 50;
  int rang = 110;
  l_servo.attach(6); //Defines left servo to attach at pin 6(can be any pin number)
  l_servo.write(lang); //opens left
  for (lang = 50; lang <= 175; lang += 5) { // goes from -5 degrees to 90 degrees
//    // in steps of 1 degree
    l_servo.write(lang);              // tell servo to go to position in variable 'pos'
    delay(55);                       // waits 15ms for the servo to reach the position
  }
  l_servo.detach();
  delay(10000);
  
  
  r_servo.attach(7); //Defines right servo to attach at pin 10(can be any pin number)
  r_servo.write(rang); //opens right
    for (rang = 110; rang >= 20; rang -= 5) {  // goes from -5 degrees to 90 degrees
//    // in steps of 1 degree
    r_servo.write(rang);              // tell servo to go to position in variable 'pos'
    delay(55);                       // waits 15ms for the servo to reach the position
  }
   r_servo.detach();
  delay(10000);

}


void servoclose(void){
  int lang = 175;
  int rang = 20;
  l_servo.attach(6); //Defines left servo to attach at pin 9(can be any pin number)
  l_servo.write(175); //closes left
  for (lang = 175; lang >= 30; lang -= 5) { // goes from 180 degrees to -5 degrees
    l_servo.write(lang);              // tell servo to go to position in variable 'pos'
    delay(55);                       // waits 15ms for the servo to reach the position
  }
  l_servo.detach();
  delay(10000);
  
  r_servo.attach(7); //Defines right servo to attach at pin 10(can be any pin number)
  r_servo.write(20); //closes right
  for (rang = 20; rang <= 110; rang += 5) {  // goes from 180 degrees to -5 degrees
    r_servo.write(rang);              // tell servo to go to position in variable 'pos'
    delay(55);                       // waits 15ms for the servo to reach the position
  }
  r_servo.detach();
  delay(10000);
}
