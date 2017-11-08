/*
  MotorControl Module

  Motor shield, Adafruit motor shield compatible
  Uses 74HC595 to control 2 x L293D H-Bridge Chipset to drive up to 4 dc motor bi-directional 
  or 2 stepper motors and 2 servo's. For full control use Adafruit library
  
  This module only controls dc motor 1 and 2 
  The shield has a latch register to control the h-bridge drivers
  
*/
//-----------------------------------------------------------------------------------------
// Pin declarations motor shield

#define MOTOR1SPEED 11 // Motor 1 speed pin (D11) PWM2A (motor board ENB)
#define MOTOR1DIR1   9 // motor 1 direction control 1 (motor board IN4)
#define MOTOR1DIR2   8 // motor 1 direction control 1 (motor board IN3)

#define MOTOR2SPEED  3 // Motor 2 speed pin (D3) PWM2B  (motor board ENA)
#define MOTOR2DIR1   4 // motor 2 direction control 1 (motor board IN1)
#define MOTOR2DIR2   5 // motor 2 direction control 1 (motor board IN2)

//-----------------------------------------------------------------------------------------
// Motor initialization

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
// motor control

void motorControl(byte motorNumber, byte motorDirection, byte motorSpeed)
{
  byte A; // Motor A bit
  byte B; // Motor B bit
  byte SpeedPin; // Motor Speed Pin
   
  if (motorNumber == 1) // motor 1
  {
    if (motorDirection == FORWARD) // forward
    {
      digitalWrite(MOTOR1DIR1, LOW); // motor board IN4 low
      digitalWrite(MOTOR1DIR2, HIGH); // motor board IN3 high
    } 
    else if (motorDirection == REVERSE)
    { 
      digitalWrite(MOTOR1DIR1, HIGH); // motor board IN4 high
      digitalWrite(MOTOR1DIR2, LOW); // motor board IN3 low
    }
    else // (motorDirection == STOP) // stop
    {
      digitalWrite(MOTOR1DIR1, LOW); // motor board IN4 low
      digitalWrite(MOTOR1DIR2, LOW); // motor board IN3 low
    }
    analogWrite( MOTOR1SPEED,  motorSpeed); // set pwm value to ENB
  }
  else //(motorNumber == 2)
  {
    if (motorDirection == FORWARD) // forward
    { 
      digitalWrite(MOTOR2DIR1, LOW); // motor board IN1 low
      digitalWrite(MOTOR2DIR2, HIGH); // motor board IN2 low
    } 
    else if (motorDirection == REVERSE) // reverse
    { 
      digitalWrite(MOTOR2DIR1, HIGH); // motor board IN1 low
      digitalWrite(MOTOR2DIR2, LOW); // motor board IN2 low
    }
    else // (motorDirection == STOP) stop
    {
      digitalWrite(MOTOR2DIR1, LOW); // motor board IN1 low
      digitalWrite(MOTOR2DIR2, LOW); // motor board IN2 low
    }
    analogWrite( MOTOR2SPEED,  motorSpeed); // set pwm value to ENA
  }
}

//-----------------------------------------------------------------------------------------
// Motor speed

void motorSpeed (byte motorNumber, byte motorPwm)
{
  if (motorNumber == 1)
    analogWrite(MOTOR1SPEED,  motorPwm); // set PWM to required motor speed
  if (motorNumber == 2)
    analogWrite(MOTOR2SPEED,  motorPwm); // set PWM to required motor speed
}

// end module
