/*

  MotorControl Module

  Motor shield, Adafruit motor shield compatible
  Uses 74HC595 to control 2 x L293D H-Bridge Chipset to drive up to 4 dc motor bi-directional 
  or 2 stepper motors and 2 servo's. For full control use Adafruit library
  
  This module only controls dc motor 1 and 2 
  The shield has a latch register to control the h-bridge drivers
  
*/
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
void motorControlInit()
{
  pinMode(MOTOR1SPEED, OUTPUT);
  pinMode(MOTOR1DIR1, OUTPUT);
  pinMode(MOTOR1DIR2, OUTPUT);

  pinMode(MOTOR2SPEED, OUTPUT);
  pinMode(MOTOR2DIR1, OUTPUT);
  pinMode(MOTOR2DIR2, OUTPUT);

}


//-----------------------------------------------------------------------------------------
void motorControl(byte motorNumber, byte motorDirection, byte motorSpeed)
{
  byte A; // Motor A bit
  byte B; // Motor B bit
  byte SpeedPin; // Motor Speed Pin
   
  if (motorNumber == 1)
  {
    if (motorDirection == FORWARD)
    { 
      digitalWrite(MOTOR1DIR1, LOW);
      digitalWrite(MOTOR1DIR2, HIGH);
    } 
    else if (motorDirection == REVERSE)
    { 
      digitalWrite(MOTOR1DIR1, HIGH);
      digitalWrite(MOTOR1DIR2, LOW);
    }
    else // (motorDirection == STOP)
    {
      digitalWrite(MOTOR1DIR1, LOW);
      digitalWrite(MOTOR1DIR2, LOW);
    }
    analogWrite( MOTOR1SPEED,  motorSpeed);
  }
  else //(motorNumber == 2)
  {
    if (motorDirection == FORWARD)
    { 
      digitalWrite(MOTOR2DIR1, LOW);
      digitalWrite(MOTOR2DIR2, HIGH);
    } 
    else if (motorDirection == REVERSE)
    { 
      digitalWrite(MOTOR2DIR1, HIGH);
      digitalWrite(MOTOR2DIR2, LOW);
    }
    else // (motorDirection == STOP)
    {
      digitalWrite(MOTOR2DIR1, LOW);
      digitalWrite(MOTOR2DIR2, LOW);
    }
    analogWrite( MOTOR2SPEED,  motorSpeed);
  }
}

//-----------------------------------------------------------------------------------------
void motorSpeed (byte motorNumber, byte motorPwm)
{
  if (motorNumber == 1)
    analogWrite(MOTOR1SPEED,  motorPwm); // set PWM to required motor speed
  if (motorNumber == 2)
    analogWrite(MOTOR2SPEED,  motorPwm); // set PWM to required motor speed
}

// end module
