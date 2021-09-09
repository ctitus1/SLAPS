
// *** idk what this does
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

/*!
* @brief Attempts to move a servo to a target angle during a specified delay. servo_move_to() intentionally pauses
* execution of the caller function while it executes by using the delay() function. If desired, this function can be modified
* to not pause execution of the caller function. Does not detatch servo at the end of execution. servo will continue to try and
* reach its target until another servo.write() function is called or it is detached. This is its own function in case there are
* fundamental changes to how the servos should move (specifically if the servos are changed to allow feedback on their
* current position).
* @param servo
*   servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
*   which will be moved.
* @param target
*   The target angle of the servo in degrees. No checks are performed to see whether target is in range of the servo's
*   capabilities.
* @param delay_ms
*   The current servo has no feedback for its current position, so rather than specify a speed or desired time of operation,
*   a simple delay is specified to allow for the servo to reach its target. THERE IS NO GUARENTEE THAT THE TARGET IS REACHED
*   because there is no way to do so solely with the chosen servo. There are also no checks to see if the delay is too short
*   for the servo to reach its target given the servo specs (it could be added relatively easily though). It is guarenteed that
*   the servo will have at least delay_ms milliseconds to reach its target before any other function can interrupt the servo.
*   It's an unsigned int because it will be used by delay() which takes an unsigned int as a parameter.
*   delay_ms has units of milliseconds.
* @return No return value.
*/
private void servo_move_to(Servo servo, int target, unsigned long delay_ms) {
  servo.write(target);
  delay(delay_ms);
}

/*!
* @brief Function to open both doors. They currently open one after another, but this can be changed by modifying servo_move_to()
* or merging it with this function. Both servo objects MUST BE ATTACHED in the caller function. 
* @param l_servo
*     l_servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
*     corresponding to the left door.
* @param r_servo
*     r_servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
*     corresponding to the right door.
* @return No return value.
*/
void open_doors(Servo l_servo, Servo r_servo) { // *** consider renaming servos or flipping them so same open and close angles
  const int l_open_ang = 175; // target angle of left servo to be open
  const unsigned long l_open_delay = 10000; // time allotted for left servo to reach target angle
  const int r_open_ang = 20; // target angle of right servo to be open
  const unsigned long r_open_delay = 10000; // time allotted for right servo to reach target angle

  servo_move_to(l_servo,l_open_ang,l_open_delay);
  servo_move_to(r_servo,r_open_ang,r_open_delay);
}

/*!
* @brief Function to close both doors. They currently close one after another, but this can be changed by modifying servo_move_to()
* or merging it with this function. Both servo objects MUST BE ATTACHED in the caller function. 
* @param l_servo
*     l_servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
*     corresponding to the left door.
* @param r_servo
*     r_servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
*     corresponding to the right door.
* @return No return value.
*/
void close_doors(Servo l_servo, Servo r_servo) { // *** consider renaming servos or flipping them so same open and close angles
  const int l_close_ang = 30; // target angle of left servo to be open
  const unsigned long l_close_delay = 10000; // time allotted for left servo to reach target angle
  const int r_close_ang = 110; // target angle of right servo to be open
  const unsigned long r_close_delay = 10000; // time allotted for right servo to reach target angle

  // *** both doors can open at the same time unless there's a valid reason not to
  servo_move_to(l_servo,l_close_ang,l_close_delay);
  servo_move_to(r_servo,r_close_ang,r_close_delay);
}
