// Function definitions

// closes door by going from whatever angle the servo is currently targeting to CLOSE_ANGLE. Takes data at each step. Whether servo detaches is determined by DETATCH_AFTER_MOVING.
void close_door() {

  servo.attach(SERVO_PIN);
  state = servo_closing;
  for (int angle = servo.read(); angle >= CLOSE_ANGLE; angle -= DEG_PER_STEP) {

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

// opens door by going from whatever angle the servo is currently targeting to OPEN_ANGLE. Takes data at each step. Whether servo detaches is determined by DETATCH_AFTER_MOVING.
void open_door() {

  servo.attach(SERVO_PIN);
  state = servo_opening;
  for (int angle = servo.read(); angle <= OPEN_ANGLE; angle += DEG_PER_STEP) {

    servo.write(angle); // tell servo to go to specified angle\
    mem_write(); // takes an initial data point as soon as memory is set up
    delay(DEG_PER_STEP * DELAY_PER_DEG); // waits for the servo to reach the position

  }

#if DETATCH_AFTER_MOVING
  servo.detach();
#endif
  state = servo_open;
  mem_write(); // takes an initial data point as soon as memory is set up

}

// initializes all sensors
void sensor_init() {
  ///////////////////////////
  // Initialize BMP Sensor //
  ///////////////////////////
  if (!bmp.begin(BMP_ADDRESS)) { // if BMP doesn't initialze

    err = bmp_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  } else { // if BMP initializes

    altitude = bmp.readAltitude(LOCAL_P_MBAR); // initialize altitude

  }
  ///////////////////////////

  ////////////////////
  // Initialize INA //
  ////////////////////
  if (! ina.begin()) {

    err = ina_err;
#if HALT_ON_FAILURE
    while (1);
#endif
  }
  ////////////////////

  ////////////////////
  // Initialize LSM //
  ////////////////////
  if (!lsm.begin()) { // if LSM doesn't initialze

    err = lsm_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  } else { // if LSM initialzes

    // set range/sensitivity of IMU
    lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);   // 2G, 4G, 6G, 8G, 16G
    lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);     // 2GAUSS, 4GAUSS, 8GAUSS
    lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS); // 245DPS, 500DPS, 2000DPS

  }
  ////////////////////

}

// initializes SD card and creates file to write to
void mem_init() {
  if (!SD.begin(SD_PIN)) { // if SD card doesn't initialize

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }

  file = SD.open(FILENAME, FILE_WRITE); // creates and names file

  if (file) { // if file opens successfully...

    // uses preprocessor derectives to determine which header(s) to write to file to conserve memory
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

  } else { // if file doesn't open successfully...

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }

}

// writes all data to memory and updates altitude global variable
void mem_write() {

  file = SD.open(FILENAME, FILE_WRITE);

  if (file) { // if file opens successfully

    lsm.read();
    altitude = bmp.readAltitude(LOCAL_P_MBAR);

    file.print(millis()); file.print(",");                        // time
    file.print(err); file.print(",");                             // err
    file.print(state); file.print(",");                           // state
    file.print(bmp.readPressure()); file.print(",");              // pressure
    file.print(bmp.readTemperature()); file.print(",");           // temp
    file.print(altitude); file.print(",");                        // altitude
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

  } else { // if file opens successfully

    err = mem_write_err;
#if HALT_ON_FAILURE
    while (1);
#endif

  }
}
