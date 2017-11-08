/*
  SpeedSensor Module
  
  Using IR led and photo diode to sense slots in the encoder disk.
  The speedSensorInit routine sets up the interupt mechanism for the chosen pin's
  The connected interrupt service routine ISR(PCINT1_vect) catches the gate's interrupt 
  and determines the pin causing the interrupt.
  
*/

//-----------------------------------------------------------------------------------------------
// speedSensor pin declarations

  #define SPEEDSENSOR1 12 // Speed sensor 1 pin D12 (motor 1 (left))
  #define SPEEDSENSOR2  2 // Speed sensor 2 pin D2 (motor 2 (right))

//-----------------------------------------------------------------------------------------------
// speedSensor global variables

  int speedSensor1Count = 0; 
  int speedSensor2Count = 0;

  int speedSensor1Speed = 0; 
  int speedSensor2Speed = 0; 

  boolean speedSensor1State = false;
  boolean speedSensor2State = false;

  unsigned long speedSensor1Time = 0; 
  unsigned long speedSensor2Time = 0; 

  char speedSensor1Direction = 1;
  char speedSensor2Direction = 1;
 
  ISRCallBack CallBack;
  

//-----------------------------------------------------------------------------------------------
void speedSensorInit( ISRCallBack CallBackPointer )
{
  CallBack = CallBackPointer;
  if(CallBack == NULL) Serial.println ("CallBackPointer NULL error!"); // show callback pointer problem
  
  pinMode(SPEEDSENSOR1, INPUT); // Speedsensor 1 pin as input
  pinMode(SPEEDSENSOR2, INPUT); // Speedsensor 2 pin as input

  // setup Pin Change Interrupt 
  PCICR  = (1<<PCIE0) | (1<<PCIE2); // Pin change Interrupt - Enable PCIE0 (PCINT0-7) port B
                                    // Pin change Interrupt - Enable PCIE2 (PCINT16-23) port D
  PCMSK0 = (1<<PCINT4);  // enable interrupt PCINT4  port B (D12)
  PCMSK2 = (1<<PCINT18); // enable interrupt PCINT18 port D (D2)


  speedSensor1State = digitalRead(SPEEDSENSOR1); // get initial value
  speedSensor2State = digitalRead(SPEEDSENSOR2); // get initial value
  
} 

//-----------------------------------------------------------------------------------------------
ISR(PCINT0_vect)  // handle interrupt for port B (D8 to D13)
{ // Speed sensor interrupt service routine (keep it short!)

  PCIFR  = (1<<PCIF0); // clear interrupt flag port B 
 
  if (digitalRead(SPEEDSENSOR1) != speedSensor1State) // sensor 1 changed?
  {
    speedSensor1State = !speedSensor1State; // store current value (HIGH/LOW)
    speedSensor1Count++; // count the slots and direction +1/-1
    speedSensor1Time = millis(); // setup for next measurement
    CallBack( 1 ); // do callback
  }
}

//-----------------------------------------------------------------------------------------------
ISR(PCINT2_vect) // handle interrupt for port D (D0 to D7)
{ // Speed sensor interrupt service routine (keep it short!)
  
  PCIFR  = (1<<PCIF2); // clear interrupt flag port D 

  if (digitalRead(SPEEDSENSOR2) != speedSensor2State) // sensor 1 changed?
  {
    speedSensor2State = !speedSensor2State; // store current value (HIGH/LOW)
    speedSensor2Count++; // count the slots and direction +1/-1
    speedSensor2Time = millis(); // setup for next measurement
    CallBack( 2 ); // do callback
  }
}

//-----------------------------------------------------------------------------------------------
void speedSensorSetDirection( byte sensorId, byte sensorDirection )
{ // set the direction for the specified sensor
  
  if (sensorId == 1) // sensor 1 / motor 1
  {
    if (sensorDirection == FORWARD) speedSensor1Direction=1; // set for forward distance count
    if (sensorDirection == REVERSE) speedSensor1Direction=-1; // set for reverse distance count
  }
  if (sensorId == 2) // sensor 2 / motor 2
  {
    if (sensorDirection == FORWARD) speedSensor2Direction=1; // set for forward distance count
    if (sensorDirection == REVERSE) speedSensor2Direction=-1; // set for reverse distance count
  }
}

//-----------------------------------------------------------------------------------------------
byte speedSensorGetDirection( byte sensorId )
{ // return the set direction for the requested sensor
  
  if (sensorId == 1) // sensor 1 / motor 1
  {
    if (speedSensor1Direction == 1) 
      return FORWARD;
    else
      return REVERSE;
  }

  if (sensorId == 2) // sensor 2 / motor 2
  {
    if (speedSensor2Direction == 1) 
      return FORWARD;
    else
      return REVERSE;
  }
}

//-----------------------------------------------------------------------------------------------
int speedSensorClear()
{ // clear speedsensor counts

  speedSensor1Count = 0;
  speedSensor2Count = 0;
}

//-----------------------------------------------------------------------------------------------
int speedSensorReadCount( byte sensorId )
{ // return the requested sensor interrupt count
  
  if (sensorId == 1)
    return (speedSensor1Count);
  if (sensorId == 2)
    return (speedSensor2Count);
}

//-----------------------------------------------------------------------------------------------
int speedSensorReadTime( byte sensorId  )
{ // return the time between the last measurement and now
  
  if (sensorId == 1)
    return (millis()-speedSensor1Time); // return ms passed since previous interrupt

  if (sensorId == 2)
    return (millis()-speedSensor2Time); // return ms passed since previous interrupt
}

// end module
