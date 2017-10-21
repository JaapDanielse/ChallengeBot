/*
  Servo Module

  Drives the servo using the standard Servo library

*/


#define SERVOMIN 100 // 
#define SERVOMAX 710 // 

static int Servo000deg = 100;
static int Servo180deg = 700;
static int Servo090deg = 300;
static float ServoDegreeStepLow = 0;
static float ServoDegreeStepHigh = 0;


void ServoInit(int MinPulse, int MidPulse, int MaxPulse)
{
  Servo000deg = constrain( MinPulse, SERVOMIN, SERVOMAX);
  Servo090deg = constrain( MidPulse, SERVOMIN, SERVOMAX);
  Servo180deg = constrain( MaxPulse, SERVOMIN, SERVOMAX);
  ServoDegreeStepLow  = (float(Servo090deg - Servo000deg) / 90.0);
  ServoDegreeStepHigh = (float(Servo180deg - Servo090deg) / 90.0);
  /*  ------------------------------------
       Initiate T/C-1 in fast PWM mode-14
      ------------------------------------
  */
  pinMode(SERVOPIN, OUTPUT);

  TCCR1A = 0;                                              // Reset Control Register A        (Page 132)
  TCCR1A = (1 << COM1B1) | (1 << WGM11);                   // Enable Fast PWM on OC1B (Pin 10)
  TCCR1B = 0;                                              // Reset Control Register B        (Page 134)
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Mode 14 (TOP = ICR1), pre-scale = 1
  ICR1 = 5000;  // T/C-1 TOP limit (5000 = 20 ms)
  TCNT1 = 0;    // T/C-1

  ServoWrite(90);
}


void ServoWrite(int Degrees)
{
  int DegreesReversed;

  DegreesReversed = 180 - Degrees;

  if ( DegreesReversed <= 90)
    OCR1B = int(Servo090deg - (float constrain((90 - DegreesReversed), 0, 180) * ServoDegreeStepLow));
  else
    OCR1B = int(Servo090deg + (float constrain((DegreesReversed - 90), 0, 180) * ServoDegreeStepHigh));
}



void ServoSweep(int SweepArray[])
{

#define SWEEPDELAY 40 //

  Serial.println ("Sweep");

  ServoWrite(90);
  delay(100);

  for ( int i = 90; i >= 0; i -= 6)
  {
    ServoWrite(i);
    delay(SWEEPDELAY);
    SweepArray[(i / 3)] = distanceSensorRead();
  }

  for ( int i = 3; i <= 180; i += 6)
  {
    ServoWrite(i);
    delay(SWEEPDELAY);
    SweepArray[(i / 3)] = distanceSensorRead();
  }

  for ( int i = 180; i > 90; i -= 6)
  {
    ServoWrite(i);
    delay(SWEEPDELAY);
    SweepArray[(i / 3)] = distanceSensorRead();
  }

  ServoWrite(90);
}


boolean SweepAnalyse( int SweepArray[], int MaxDistance, int MinWidth, int *Direction, int *Distance)
{

  int ClosestDistance = 200;
  int ClosestDirection = 0;
  int ClosestCount = 0;

  for ( int i = 0; i <= 60; i++)
  {

#ifdef DEBUG
    Serial.print (i);
    Serial.print (" - ");
    Serial.print (i * 3);
    Serial.print ("' - ");
    Serial.print (SweepArray[(i)]);
#endif  

    if (SweepArray[i] != 0)
    {
      if (( SweepArray[i - 1] < SweepArray[i] - 5 ||
            SweepArray[i - 1] > SweepArray[i] + 5 )
          &&
          ( SweepArray[i + 1] < SweepArray[i] - 5 ||
            SweepArray[i + 1] > SweepArray[i] + 5 )
         )
      {
#ifdef DEBUG
        Serial.print (" - noise");
#endif  
      }
      else
      {
        if (SweepArray[i] < ClosestDistance - 5 && SweepArray[i] != 0 )
        {
          ClosestDirection = i * 3;
          ClosestDistance = SweepArray[i];
          ClosestCount = 1;
        }
        else if ( SweepArray[i] > ClosestDistance - 3 &&
                  SweepArray[i] < ClosestDistance + 3 &&
                  ClosestDirection == ((i - ClosestCount) * 3))
        {
          if ( SweepArray[i] < ClosestDistance ) ClosestDistance = SweepArray[i];
          ClosestCount++;
        }
      }
    }
#ifdef DEBUG
    Serial.println();
#endif  
  }
  ClosestDirection += ((ClosestCount * 3) / 2);

#ifdef DEBUG
  Serial.print ("Closest: ");
  Serial.print (ClosestDirection);
  Serial.print ("' - dist:");
  Serial.print (ClosestDistance);
  Serial.print (" - wide:");
  Serial.println (ClosestCount);

  Serial.print (">> :");
  Serial.println ( 90 - (180 - ClosestDirection));
#endif  


  if ( ClosestDistance < MaxDistance && ClosestCount >= MinWidth  )
  {
    ServoWrite(ClosestDirection);
    delay (500);

    *Direction = 90 - (180 - ClosestDirection);
    *Distance = ClosestDistance * 10;

    return true;
  }
  return false;
}










