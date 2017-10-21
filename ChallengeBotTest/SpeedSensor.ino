/*
  SpeedSensor Module
  
  Using IR led and photo diode to sense slots in the encoder disk.
  The speedSensorInit routine sets up the interupt mechanism for the chosen pin's
  The connected interrupt service routine ISR(PCINT1_vect) catches the gate's interrupt 
  and determines the pin causing the interrupt.
  
*/

//-----------------------------------------------------------------------------------------------
// speedSensor pin declarations



//-----------------------------------------------------------------------------------------------
// speedSensor global variables

  int speedSensor1Count = 0; 
  int speedSensor2Count = 0;

  int speedSensor1Speed = 0; 
  int speedSensor2Speed = 0; 

  unsigned long speedSensor1Time = 0; 
  unsigned long speedSensor2Time = 0; 


  boolean speedSensor1State = false;
  boolean speedSensor2State = false;


//-----------------------------------------------------------------------------------------------
void speedSensorInit()
{
  pinMode(SPEEDSENSOR1, INPUT); // Speedsensor 1 pin as input
  pinMode(SPEEDSENSOR2, INPUT); // Speedsensor 2 pin as input


  // setup Pin Change Interrupt for A and B port 
  PCIFR = (1<<PCIF0) | (1<<PCIF2); // A and B port Intterupt Clear
  PCICR = (1<<PCIE0) | (1<<PCIE2); // Pin change Interrupt - Enable port A and B

  PCMSK0 = (1<<PCINT4); // enable interrupt D12
  PCMSK2 = (1<<PCINT18); // enable interrupt D2


  speedSensor1State = digitalRead(SPEEDSENSOR1); // get initial value
  speedSensor2State = digitalRead(SPEEDSENSOR2); // get initial value

} 

//-----------------------------------------------------------------------------------------------
ISR(PCINT0_vect)  // handle interrupt for D8 to D13
{ // Speed sensor interrupt service routine (keep it short!)
  
  if (digitalRead(SPEEDSENSOR1) != speedSensor1State) // sensor 1 changed?
  {
    speedSensor1State = !speedSensor1State; // store current value (HIGH/LOW)
    if(speedSensor1State) // only react on becomming high (start of sensor slot)
    {
      speedSensor1Count++;
      speedSensor1Speed = (float(1000.0/(millis()-speedSensor1Time))*10); // calculate speed in mm/s (one slot =  10.5 mm. movement)
      speedSensor1Time = millis(); // setup for next measurement

    }
  }
}

//-----------------------------------------------------------------------------------------------
ISR(PCINT2_vect) // handle interrupt for D0 to D7
{
  // interrupt service routine (keep it short!)
  
  if (digitalRead(SPEEDSENSOR1) != speedSensor1State) // sensor 1 changed?
  {
    speedSensor1State = !speedSensor1State; // store current value (HIGH/LOW)
    if(speedSensor1State) // only react on becomming high (start of sensor slot)
    {
      speedSensor1Count++;
    }
  }

  if (digitalRead(SPEEDSENSOR2) != speedSensor2State) // sensor 2 changed?
  {
    speedSensor2State = !speedSensor2State; // store current value (HIGH/LOW)
    if(speedSensor2State) // only react on becomming high (start of sensor slot)
    {
      speedSensor2Count++;
      speedSensor2Speed = (float(1000.0/(millis()-speedSensor2Time))*10); // calculate speed in mm/s (one slot =  10.5 mm. movement)
      speedSensor2Time = millis(); // setup for next measurement

    }
  } 
}


//-----------------------------------------------------------------------------------------------
int speedSensorReadCount( byte sensorId )
{
  if (sensorId == 1)
    return (speedSensor1Count);
  if (sensorId == 2)
    return (speedSensor2Count);
}


//-----------------------------------------------------------------------------------------------
int speedSensorReadSpeed( byte sensorId )
{
  if (sensorId == 1)
    return (speedSensor1Speed);
  if (sensorId == 2)
    return (speedSensor2Speed);
}

//-----------------------------------------------------------------------------------------------
int speedSensorClear()
{
    speedSensor1Count = 0;
    speedSensor2Count = 0;
}

// end module
