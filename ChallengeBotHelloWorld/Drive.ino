/*
 
 Drive Module
 
 The drive math:
 The speed sensor wheels have 20 slots and 20 spokes.
 Since all changes in the sensor pins result in an interrupt 
 there will be 40 interrupts for a full rotation of a wheel
 
 Te wheel circumference is 210 mm. 
 One interrupt is therefore equal to 210/20 = 5.25 mm. movement.
 
 When turning the distance between the wheel centers is 140 mm.

 If only one wheel runs a 180 deg. turn equals:
 (2*pi*140)/2 = 440 (rounded)  (the /2 is half a turn = 180 deg)

 If both wheels are running in opposite direction the center of the circle will be in the middle.
 and each wheel has to travel half the distance (220 mm)

 If a wheel has to travel 220 mm then it will have 220/5.25 = 42 (rounded) interrupts 
 For a two wheel turn this means a factor of 180/42 = 4.3 degrees per interrupt applies.

 Some how this falls a bit short (dont know why) so i added a correcion factor

*/

//-----------------------------------------------------------------------------------------------
// defines

// drive status value
#define IDLING   0 // 
#define STOPPED  1 // 
#define DRIVING  3 //
#define TURNING  4 //

// maximum stall time
#define STALLTIME 20 // maximum allowed time between interrupt, if exeeded stall is caught

// distance and turn calculation factors
#define MMPERINTERRUPT 5.25 // number of millimeters per interrupt
#define DEGPERINTERRUPT 4.30 // degrees to interrupt factor

// drive global variables
byte driveStatus = STOPPED;  
int driveMotorPwm = 0; // pwm value 
int driveInterruptsToDo = 0; // total number of interrupts to handle
int driveSensor1Count = 0; // speed sensor 1 interrupt counter for action
int driveSensor2Count = 0; // speed sensor 2 interrupt counter for action

//-----------------------------------------------------------------------------------------------
boolean driveStraight(byte direction, int distance, int driveSpeed, byte obstacleDistWarning )
{ // drive in a straight line for a given distance with a given speed. Watch for obstacles closer then given distance
  
  driveInterruptsToDo = int( float(distance*10)/MMPERINTERRUPT ); // calculate number of indexwheel interrupts

  #ifdef DEBUG
    Serial.print("Drive: "); 
    Serial.print(distance); 
    Serial.print(" cm. I-cnt: "); 
    Serial.println(driveInterruptsToDo); 
  #endif  

  if (driveSpeed == SLOW) // determine PWM value
    driveMotorPwm = SLOWPWM; // set to slow pwm
  else  
    driveMotorPwm = FASTPWM; // set to fast pwm

  driveSensor2Count = 0; // clear speed sensor interrupt Count
  driveSensor1Count = 0; // clear speed sensor interrupt Count

  driveStatus = DRIVING; // we are driving

  speedSensorSetDirection(1, direction); // set direction for the sensors
  speedSensorSetDirection(2, direction); // set direction for the sensors

  motorControl(1, direction, FASTPWM); // output to motor (start with fast pwm)
  motorControl(2, direction, FASTPWM); // output to motor (start with fast pwm)
  
  while( driveSensor1Count < driveInterruptsToDo && driveSensor2Count < driveInterruptsToDo ) // do while not complete  
  {
    Serial.print(""); // needed to solve arduino stack bug
    delay(STALLTIME/3); // wait a bit
    driveCatchStall(); // catch stall 
    if ( distanceSensorCheckObstacle(obstacleDistWarning) ) // check for obstacles
    { // obstacle detected within range 
      driveStatus = IDLING; // no longer driving
      return false; // signal obstacle detected
    }
  }
  
  driveStatus = IDLING; // no longer driving
  return true; // normal exit
  
}


//-----------------------------------------------------------------------------------------------
void driveTurn(int turnDegrees)
{ // make a turn for a (approximate) given number of degrees

  int driveSpeed = SLOW;

  driveInterruptsToDo = int( float( abs(turnDegrees)/(DEGPERINTERRUPT + TURNCORRECTION)));

  #ifdef DEBUG
    Serial.print("Turn: "); 
    Serial.print(turnDegrees); 
    Serial.print(" deg. I-cnt: "); 
    Serial.println(driveInterruptsToDo); 
  #endif  

  driveSensor1Count = 0; // clear speed sensor interrupt Count
  driveSensor2Count = 0; // clear speed sensor interrupt Count

  driveMotorPwm = SLOWPWM; // set slow pwm
  driveStatus = TURNING; // we are turning

  if (turnDegrees >= 0)
  { // right turn
    speedSensorSetDirection(1, FORWARD); // set direction for the sensors
    speedSensorSetDirection(2, REVERSE); // set direction for the sensors
    motorControl(1, FORWARD, FASTPWM); // start fast, output to motor
    motorControl(2, REVERSE, FASTPWM); // start fast output to motor
  }
  else
  { // left turn
    speedSensorSetDirection(1, REVERSE); // set direction for the sensors
    speedSensorSetDirection(2, FORWARD); // set direction for the sensors
    motorControl(1, REVERSE, FASTPWM); // output to motor
    motorControl(2, FORWARD, FASTPWM); // output to motor
  }
    
  while( driveSensor1Count < driveInterruptsToDo && driveSensor2Count < driveInterruptsToDo ) // do while not complete
  { 
    Serial.print(""); // needed to solve arduino stack bug
    delay(STALLTIME/3); // wait a bit
    driveCatchStall(); // catch stall
  }

  driveStatus = IDLING; // no longer turning
  
  return false;
  
}


//-----------------------------------------------------------------------------------------------
void driveStop()
{ // stop straight drive or turn by shortly reversing motors
  // one of the wheels has completed its distance
  
  if ( speedSensorGetDirection(1)==FORWARD ) // check direction and reverse
    motorControl(1, REVERSE, SLOWPWM); // output to motor
  else
    motorControl(1, FORWARD, SLOWPWM); // output to motor
  
  if ( speedSensorGetDirection(2)==FORWARD ) // check direction and reverse
    motorControl(2, REVERSE, SLOWPWM); // output to motor
  else
    motorControl(2, FORWARD, SLOWPWM); // output to motor
  
  delay(80); // wait for stop

  motorControl(1, speedSensorGetDirection(1), 0); // reset to original direction
  motorControl(2, speedSensorGetDirection(2), 0); // reset to original direction

  driveStatus = STOPPED; // we hav stopped
  
}


//-----------------------------------------------------------------------------------------------
int driveDistanceDone()
{ // returns the distance driven 
  return ((int (float( driveSensor1Count + driveSensor2Count) / 2.0) * MMPERINTERRUPT)/10) ;
}


//-----------------------------------------------------------------------------------------------
void driveSpeedSensorCallback(byte SensorId )
{ // called on interrupt of the speed sensors. Switches motors on or off depending on speed and other wheel interrupt count
  // extends interrupt service routine: KEEP IT SHORT!

  if (SensorId == 1) 
    driveSensor1Count++; // keep track of this action
  else // SensorId == 2
    driveSensor2Count++; // keep track of this action

  #ifdef DEBUG
    Serial.print("S:"); // print in debug 
    Serial.print(SensorId); // sensor id
    Serial.print(" 1C: "); // Sensor 1 count
    Serial.print(driveSensor1Count); 
    Serial.print(" 2C: "); // Sensor 2 count
    Serial.print(driveSensor2Count);
  #endif  
  
  if (driveStatus == DRIVING || driveStatus == TURNING)
  { 
    if ( driveSensor1Count <= driveSensor2Count )
    { // we are on speed or slow and not ahead of motor 2
      motorSpeed(1, driveMotorPwm); // too slow: engine on
      #ifdef DEBUG
        Serial.print(" M1+"); // motor 1 start
      #endif  
    }
    else
    { // we are too fast or ahead of motor 2
      motorSpeed(1, 0); // too fast: engine off
      #ifdef DEBUG
        Serial.print(" M1-"); // motor 1 stop
      #endif  
    }

    if ( driveSensor2Count <= driveSensor1Count )
    { // we are on speed or slow and not ahead of motor 1
      motorSpeed(2, driveMotorPwm); // store the initial drive speed
      #ifdef DEBUG
        Serial.print(" M2+"); // motor 2 start
      #endif  
    }
    else
    { // we are too fast or ahead of motor 1
      motorSpeed(2, 0); // store the initial drive speed
      #ifdef DEBUG
        Serial.print(" M2-"); // motor 2 stop
      #endif  
    }
  }

  #ifdef DEBUG
    Serial.println(""); 
  #endif  
}


//-----------------------------------------------------------------------------------------------
void driveCatchStall()
{ // called from driveStraight and driveTurn to catch a stalling engine start it again.
  
    if ( speedSensorReadTime(1) > STALLTIME ) // have we stalled?
    {
      motorSpeed(1, FASTPWM); // yes: start moving again
#ifdef DEBUG
     Serial.print(" Stall 1 "); // show stall in debug
#endif  
    }
    
    if ( speedSensorReadTime(2) > STALLTIME ) // have we stalled?
    {
      motorSpeed(2, FASTPWM); // yes: start moving again
#ifdef DEBUG
      Serial.print(" Stall 2 "); // show stall in debug
#endif  
    }
}

// end module
