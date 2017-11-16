/*
  Servo Module

  Drives the servo using the standard Servo library
  Does scan sweep and analyses result.

*/

//-----------------------------------------------------------------------------------------
// Servo hardware declaration
#define SERVOPIN  10 //

// Servo min / max values
#define SERVOMIN 100 // minimum pwm value
#define SERVOMAX 710 // maximum pwm value

//-----------------------------------------------------------------------------------------
// Servo global variables
static int   Servo000deg = 100; // 0 degree pwm value
static int   Servo180deg = 700; // 180 degree pwm value
static int   Servo090deg = 300; // 90 degree pwm value
static float ServoDegreeStepLow = 0; // pwm value per degree for 90 - 0 deg.
static float ServoDegreeStepHigh = 0; // pwm value per degree for 90 - 180 deg.


//-----------------------------------------------------------------------------------------
// Servo initialisation

void ServoInit(int MinPulse, int MidPulse, int MaxPulse)
{
  Servo000deg = constrain( MinPulse, SERVOMIN, SERVOMAX); // set 0 deg pwm
  Servo090deg = constrain( MidPulse, SERVOMIN, SERVOMAX); // set 90 deg pwm
  Servo180deg = constrain( MaxPulse, SERVOMIN, SERVOMAX); // set 180 deg pwm

  ServoDegreeStepLow  = (float(Servo090deg - Servo000deg) / 90.0); // calculate pwm degree step low
  ServoDegreeStepHigh = (float(Servo180deg - Servo090deg) / 90.0); // calculate pwm degree step high

  pinMode(SERVOPIN, OUTPUT); // set servo pin to output (fast pwm 16 bit)

  // Initiate Timer Counter 1 in fast PWM mode-14
  TCCR1A = 0; // Reset Control Register A 
  TCCR1A = (1 << COM1B1) | (1 << WGM11); // Enable Fast PWM on OC1B (Pin 10)
  
  TCCR1B = 0;  // Reset Control Register B 
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Mode 14 (TOP = ICR1), pre-scale = 1
  ICR1 = 5000;  // T/C-1 TOP limit (5000 = 20 ms cycle)
  TCNT1 = 0;    // T/C-1 

  ServoWrite(90); // look straight ahead
}

//-----------------------------------------------------------------------------------------
// Write degree position to servo

void ServoWrite(int Degrees)
{
  int DegreesReversed;

  DegreesReversed = 180 - Degrees; // servo is upside down. Reverse direction.

  if ( DegreesReversed <= 90) // below 90 deg: use low step
    OCR1B = int(Servo090deg - (float constrain((90 - DegreesReversed), 0, 180) * ServoDegreeStepLow)); // set val to OCR1B reg.
  else // above 90 deg: use high step
    OCR1B = int(Servo090deg + (float constrain((DegreesReversed - 90), 0, 180) * ServoDegreeStepHigh)); // set val to OCR1B reg.
}


//-----------------------------------------------------------------------------------------
// Do a sweep while measuring distance 
// Start center(90) and sweep back to 0, forward to 180 and back to 90 storing measurements interlaced in 3 deg interval.

void ServoSweep(int SweepArray[])
{

  #define SWEEPDELAY 40 //

  Serial.println ("Sweep"); // show sweep

  ServoWrite(90); // start straight ahead
  delay(100); // wait for servo to move

  for ( int i = 90; i >= 0; i -= 6) // move from 90 to 0 in 6 degree steps
  {
    ServoWrite(i); // set position
    delay(SWEEPDELAY); // wait for servo
    SweepArray[(i / 3)] = distanceSensorRead(); // read and store result
  }

  for ( int i = 3; i <= 180; i += 6) // 3 deg shift move from 3 deg to 177 deg in 6 degree steps
  {
    ServoWrite(i); // set position
    delay(SWEEPDELAY); // wait for servo to move
    SweepArray[(i / 3)] = distanceSensorRead(); // read and store result
  }

  for ( int i = 180; i > 90; i -= 6) // move back from 180 (again 3 degree shift) to 90 deg in 6 degree steps
  {
    ServoWrite(i); // set position
    delay(SWEEPDELAY); // wait for servo to move
    SweepArray[(i / 3)] = distanceSensorRead(); // read and store result
  }

  ServoWrite(90); // look straight ahead again
}

//-----------------------------------------------------------------------------------------
// Analyse 

boolean SweepAnalysis( int SweepArray[], int MaxDistance, int MinWidth, int *Direction, int *Distance)
{
  #define OBSTACLETOLERANCE 3 //
  
  int ClosestDistance = 200;
  int ClosestDirection = 0;
  int ClosestCount = 0;

  for ( int i = 0; i <= 60; i++)
  {

#ifdef DEBUG
    Serial.print (i); // show analysis in debug.
    Serial.print (" - ");
    Serial.print (i * 3); // direction in degree
    Serial.print ("' - "); 
    Serial.print (SweepArray[(i)]); // value
#endif  

    if (SweepArray[i] != 0) // ignore 0 value
    { // if value before and value after have more than 5 cm difference ignore as noise
      if (( SweepArray[i - 1] < SweepArray[i] - 5 ||
            SweepArray[i - 1] > SweepArray[i] + 5 )
          &&
          ( SweepArray[i + 1] < SweepArray[i] - 5 ||
            SweepArray[i + 1] > SweepArray[i] + 5 )
         )
      {
#ifdef DEBUG
        Serial.print (" - noise"); // show noise
#endif  
      }
      else
      { // it is not noise!
        if (SweepArray[i] <= ClosestDistance - OBSTACLETOLERANCE && SweepArray[i] != 0 ) 
        { // if sensorvalue is smaler then closest distance - tolerance range and not 0 make it the new closest dist.
          ClosestDirection = i * 3; // calculate degrees
          ClosestDistance = SweepArray[i]; 
          ClosestCount = 1;
        }
        else if ( SweepArray[i] > ClosestDistance - OBSTACLETOLERANCE && 
                  SweepArray[i] < ClosestDistance + OBSTACLETOLERANCE &&
                  ClosestDirection == ((i - ClosestCount) * 3))
        { // if within tolerance range count as same obstacle
          if ( SweepArray[i] < ClosestDistance ) ClosestDistance = SweepArray[i];
          ClosestCount++;
        }
      }
    }
    #ifdef DEBUG
      Serial.println(); // line end
    #endif  
  }
  ClosestDirection += ((ClosestCount * 3) / 2); // calculate center of the colsest object

  #ifdef DEBUG
    Serial.print ("Closest: "); // show results
    Serial.print (ClosestDirection);
    Serial.print ("' - dist:");
    Serial.print (ClosestDistance);
    Serial.print (" - wide:");
    Serial.println (ClosestCount);
    Serial.print (">> :");
    Serial.println ( 90 - (180 - ClosestDirection));
  #endif  

  if ( ClosestDistance < MaxDistance && ClosestCount >= MinWidth  ) // if within range and big enough we have a target.
  {
    ServoWrite(ClosestDirection); // look at target
    delay (500); // wait half a second

    *Direction = 90 - (180 - ClosestDirection); // write direction
    *Distance = ClosestDistance; // write distance

    return true; // target found 
  }
  return false; // nothing there
}










