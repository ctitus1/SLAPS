/*!
  @brief Helper function to set global variable "filename" to the form "[base_filename]###[ext]"
  @param num
    The number to take the place of ###. If num is less than 3 digits, it will be padded by zeroes. Function will break
    if num > 999, so if it is the function sets filename to default_filename;
  @return No return value.
*/
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

/*!
  @brief Initializes the file and sets the filename to be used for the test. If "[base_filename]000[ext]" exists, 000 will
  increment until it has a unqiue value to prevent overwriting. If ### > 999, data is stored to default_filename. Data is
  formatted as a CSV, but it is stored in whatever filetype is specified by ext (a global constant in launch_code). Headers
  must be manually updated if more data is going to be collected.
  @return No return value.
*/
void mem_init() {

  // column headers for CSV
  String col_headers = "state,current_time (ms),pressure (Pa),temp (C),altitude (m),solar_voltage (V),servo_angle (deg)";

  // increments ### until its's a unique file
  int num = 0; // num refers to ### in description, ### is just to show that it's padded by 3 zeroes
  do {
    file_num(num++);
    Serial.println(filename);
  } while (SD.exists(filename));

  // opens file to write
  file = SD.open(filename, FILE_WRITE);
  if (file) { // file opened successfully
    file.print(col_headers);
    file.println();
  } else { // file opened unsuccessfully
    Serial.print("error opening "); Serial.print(filename);
    Serial.println();
  }

  // closes and saves file
  file.close();
}

/*!
  @brief Writes the data to filename. Data is formatted as a CSV, but it is stored in whatever filetype is specified in
  filename. If more data is added, be sure to update the headers in mem_init().
  @return No return value.
*/
void mem_write() {

  // declares data variables
  unsigned long current_time;
  float pressure, temp, altitude, solar_voltage;
  int  servo_angle;
  file = SD.open(filename, FILE_WRITE);
  if (file) { // file opened successfully

    // loads data into appropriate variables
    current_time = millis();
#if BMP
    pressure = bmp.readPressure(); // *** BMP NOT WORKING FOR CHRIS
    temp = bmp.readTemperature();
    altitude = bmp.readAltitude();
#endif
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

/*!
  @brief Attempts to move a servo to a target angle during a specified delay. servo_move_to() intentionally pauses
  execution of the caller function while it executes by using the delay() function. If desired, this function can be modified
  to not pause execution of the caller function. Does not detatch servo at the end of execution. servo will continue to try and
  reach its target until another servo.write() function is called or it is detached. This is its own function in case there are
  fundamental changes to how the servos should move (specifically if the servos are changed to allow feedback on their
  current position).
  @param servo
    servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE. This is the servo object
    which will be moved.
  @param target
    The target angle of the servo in degrees. No checks are performed to see whether target is in range of the servo's
    capabilities.
  @param delay_ms
    The current servo has no feedback for its current position, so rather than specify a speed or desired time of operation,
    a simple delay is specified to allow for the servo to reach its target. THERE IS NO GUARENTEE THAT THE TARGET IS REACHED
    because there is no way to do so solely with the chosen servo. There are also no checks to see if the delay is too short
    for the servo to reach its target given the servo specs (it could be added relatively easily though). It is guarenteed that
    the servo will have at least delay_ms milliseconds to reach its target before any other function can interrupt the servo.
    It's an unsigned int because it will be used by delay() which takes an unsigned int as a parameter.
    delay_ms has units of milliseconds.
  @return No return value.
*/
void servo_move_to(Servo s_servo, int s_target, unsigned long s_delay_ms) {
  s_servo.write(s_target);
  delay(s_delay_ms);
}

/*!
  @brief Function to open all doors. All servo objects MUST BE ATTACHED in the caller function.
  @param servo
      servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE.
  @return No return value.
*/
void open_doors(Servo servo) { // *** consider renaming servos or flipping them so same open and close angles
  const int open_ang = 175; // target angle of left servo to be open
  const unsigned long open_delay = 10000; // time allotted for left servo to reach target angle

  servo_move_to(servo, open_ang, open_delay);
}

/*!
  @brief Function to open all doors. All servo objects MUST BE ATTACHED in the caller function.
  @param servo
      servo MUST BE ATTACHED in the caller function, and the code WILL NOT WORK OTHERWISE.
  @return No return value.
*/
void close_doors(Servo servo) { // *** consider renaming servos or flipping them so same open and close angles
  const int close_ang = 30; // target angle of left servo to be open
  const unsigned long close_delay = 10000; // time allotted for left servo to reach target angle

  servo_move_to(servo, close_ang, close_delay);
}
