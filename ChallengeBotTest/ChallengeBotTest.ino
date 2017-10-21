/*
    ChallengeBotTest (c)J.C. Daniëlse - October 2017
 
    Hardware test for the ChallengeBot Autonomous Robot

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


  V1.0   JnD & JpD 21-10-2017
         Original version based on  

*/

// Pin declarations Ultrasonic sensor
#define DISTANCESENSECHO  6 // distance sensor echo pin
#define DISTANCESENSTRIG  7 // distance sensor trigger pin

// Pin declarations Speed sensors
#define SPEEDSENSOR1     12 // Speed sensor 1 pin (motor 1)
#define SPEEDSENSOR2      2 // Speed sensor 2 pin (motor 2)

// Pin declarations motor board
#define MOTOR1SPEED      11 // Motor 1 speed pin (D11) PWM2A
#define MOTOR1DIR1        9 // IN3
#define MOTOR1DIR2        8 // IN4

#define MOTOR2SPEED       3 // Motor 2 speed pin (D3) PWM2B
#define MOTOR2DIR1        4 // IN1
#define MOTOR2DIR2        5 // IN2

// Pin declaration Servo
#define SERVOPIN         10 // Servo control (PWM)

//-----------------------------------------------------------------------------------------
// Motor Direction Values

#define STOP     0 //
#define FORWARD  1 //
#define REVERSE  2 //


int Servo0 = 110;
int Servo90 = 325;
int Servo180 = 590;

//-----------------------------------------------------------------------------------------

void setup() 
{

  boolean sensorOk = false;
  int Motor1PWM;
  int Motor2PWM;
  int Motor1Speed;
  int Motor2Speed;
 
  speedSensorInit();
  distanceSensorInit();
  motorControlInit();
  ServoInit(110, 325, 590); // pwm value for 0 degrees min 100, 90 degrees and 180 degrees (max 700)
                            // allows to have the 90 degree position straight ahead. 

  Serial.begin(9600); // set up Serial library at 115200 bps
  Serial.println(F("ChalengeBotTest"));
  Serial.println(F("Checking Autonomous Robot hardware"));
  Serial.println(F(""));
  Serial.println(F("Place the robot from the ground so the wheels can turn freely. And switch on battery power."));
  Serial.println(F("Ready? [y/n]"));
  while (!getReply());

 
  Serial.println(F(""));
  Serial.println(F("Testing motor 1"));
  Serial.println(F("- Motor 1 Forward"));

  speedSensorClear();
  motorControl(1, FORWARD, 255);
  delay(2000);
  motorControl(1, STOP, 0);

  Serial.println(F("Did motor 1 (left) move forward? [y/n]"));
  if (!getReply())
  {
     Serial.println(F("No - Switch off, disconnect USB and check connections"));
     while(1);
  }
  else
  {
     Serial.print(F("S1: "));
     Serial.print(speedSensorReadCount(1));
     Serial.print(F(" S2: "));
     Serial.print(speedSensorReadCount(2));
     Serial.println();
     
     Serial.println(F("Yes - Checking Speedsensor 1"));
     if (speedSensorReadCount(1) > 10)
       Serial.println(F("Speedsensor 1 - OK"));
     else
     {
       Serial.println(F("Speedsensor 1 - Not read - check connections"));
       if (speedSensorReadCount(2) > 10)
         Serial.println(F("Speedsensor 2 - Read: sensor 2 connected to motor 1 "));
       while(1);
     }
 }
 
  Serial.println(F(""));
  Serial.println(F("- Motor 1 Reverse"));
  speedSensorClear();
  motorControl(1, REVERSE, 255);
  delay(2000);
  motorControl(1, STOP, 0);
  if (speedSensorReadCount(1) < 10)
  {
     Serial.println(F("Speedsensor 1 - showed no movement: motor shield error"));
     while(1);
  }
  Serial.println(F("Did motor 1 (left) move backward? [y/n]"));
  if (!getReply())
  {
     Serial.println(F("No:  Motor Board or connection error"));
     while(1);
  }
  
   
  Serial.println(F(""));
  Serial.println(F("Testing motor 2"));
  Serial.println(F("- Motor 2 Forward"));

  speedSensorClear();
  motorControl(2, FORWARD, 255);
  delay(2000);
  motorControl(2, STOP, 0);

  Serial.println(F("Did motor 2 (right) move forward? [y/n]"));
  if (!getReply())
  {
     Serial.println(F("No - Switch off, disconnect USB and check connections"));
     while(1);
  }
  else
  {
     Serial.println(F("Yes - Checking Speedsensor 2"));

    if (speedSensorReadCount(2) > 10)
       Serial.println(F("Speedsensor 2 - OK"));
     else
     {
       Serial.println(F("Speedsensor 2 - Not read - check connections"));
       if (speedSensorReadCount(1) > 10)
         Serial.println(F("Speedsensor 1 - Read: sensor 1 connected to motor 2 "));
       while(1);
     }
  }
  
  Serial.println(F(""));
  Serial.println(F("- Motor 2 Reverse"));
  speedSensorClear();
  motorControl(2, REVERSE, 255);
  delay(2000);
  motorControl(2, STOP, 0);
  if (speedSensorReadCount(2) < 10)
  {
     Serial.println(F("Speedsensor 2 - showed no movement: motor shield error"));
     while(1);
  }
  Serial.println(F("Did motor 2 (Right) move backward? [y/n]"));
  if (!getReply())
  {
     Serial.println(F("No:  Motor Board or Connection error"));
     while(1);
  }

   Serial.println(F("Testing motor 1 minimum PWM"));
   delay(500);
   for( Motor1PWM=255; Motor1PWM>=0; Motor1PWM-=10 )
   {
     motorControl(1, FORWARD, Motor1PWM);
     delay(250);
     speedSensorClear();
     delay(250);
     Serial.print(F(""));
     if ( speedSensorReadCount(1) < 8 )
     {
       break;
     }
   }
   motorControl(1, FORWARD, 255);
   delay(250);
   motorControl(1, FORWARD, Motor1PWM);
   delay(3000);
   speedSensorClear();
   delay(250);
   Motor1Speed = speedSensorReadSpeed(1);
   motorControl(1, STOP, 0);

   Serial.print(F("Motor 1 - Min. PWM value:"));
   Serial.print( Motor1PWM + 25 );
   Serial.print(F(" Speed:"));
   Serial.print( speedSensorReadSpeed(1));
   Serial.println(F(" mm/s"));

   delay(1000);
   
   Serial.println(F("Testing motor 2 minimum PWM"));
   delay(500);
   for( Motor2PWM=255; Motor2PWM>=0; Motor2PWM-=10 )
   {
     motorControl(2, FORWARD, Motor2PWM);
     delay(250);
     speedSensorClear();
     delay(250);
     Serial.print(F(""));
     if ( speedSensorReadCount(2) < 8 )
     {
       break;
     }
   }

   motorControl(2, FORWARD, 255);
   delay(250);
   motorControl(2, FORWARD, Motor2PWM);
   delay(3000);
   speedSensorClear();
   delay(250);
   Motor2Speed = speedSensorReadSpeed(2);
   motorControl(2, STOP, 0);

   Serial.print(F("Motor 2 - min. PWM value:"));
   Serial.print( Motor2PWM + 25 );
   Serial.print(F(" Speed:"));
   Serial.print( Motor2Speed );
   Serial.println(F(" mm/s"));

   if( Motor1PWM+25 > 110) Serial.println(F("Motor 1 requires a relative high PWM value. Maybe it has friction"));
   if( Motor2PWM+25 > 110) Serial.println(F("Motor 2 requires a relative high PWM value. Maybe it has friction"));

   Serial.print(F("Please write down as SLOW pwm value: "));
   if( Motor1PWM > Motor2PWM )
     Serial.println( Motor1PWM + 25 );
   else
     Serial.println( Motor2PWM + 25 );

  Serial.println(F(""));
  Serial.println(F("Testing Distance Sensor"));
  Serial.println(F("Hold your hand in front of the sensor"));
  Serial.println(F("Ready? [y/n]"));
  while (!getReply());

  sensorOk = false;
  for(int i=0; i<20; i++)
  { 
    int dist;

    dist = distanceSensorRead();   

    Serial.print( dist);
    Serial.println(F(" cm."));
    
    if(dist > 0 && dist < 40) sensorOk = true;   
    delay(250);
  }

  if (sensorOk)
  {
     Serial.println(F("Distance sensor reading OK"));
  }
  else
  {
     Serial.println(F("Distance sensor FAIL"));
     Serial.println(F("Switch off, disconnect USB and check connections"));
     while(1);
  }
  Serial.println(F("Testing Servo"));

  ServoWrite(0);
  delay(250);
  for(int i=0; i<180; i+=5)
  {
     ServoWrite(i);
     delay(50);
  }
  delay(250);
  ServoWrite(90);
  
  Serial.println(F("Did the sevo sweep [y/n]"));
  if (!getReply())
  {
     Serial.println(F("No - Switch off, disconnect USB and check connections"));
     while(1);
  }

  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F("Congratulations! Your robot seems to be working"));

  Serial.println(F(""));

   Serial.println(F("Now we need to calibrate the Servo angles"));
   
   ServoWrite(90);
   delay(250);
   Serial.println(F("Is the servo facing straight forward? {y/n]"));
   while(!getReply())
   {
     Serial.println(F("Enter L for left R for right and S for stop [l/r/s]"));
     while(1)
     {
       int Reply = getIntReply();
       if ( Reply==1 ) Servo90 += 5;
       if ( Reply==2 ) Servo90 -= 5;
       if ( Reply==3 ) break;
       ServoInit(Servo0, Servo90, Servo180); // pwm value for 0 degrees(min 100), 90 degrees and 180 degrees (max 700)
       ServoWrite(90);
       delay(250);
     }
     Serial.println(F("Is the servo now facing (exactly) straight forward? {y/n]"));
   }

   ServoWrite(0);
   delay(250);
   Serial.println(F("Is the servo facing 90 degrees left? {y/n]"));
   while(!getReply())
   {
     Serial.println(F("Enter L for left R for right and S for stop [l/r/s]"));
     while(1)
     {
       int Reply = getIntReply();
       if ( Reply==1 ) Servo180 += 5;
       if ( Reply==2 ) Servo180 -= 5;
       if ( Reply==3 ) break;
       ServoInit(Servo0, Servo90, Servo180); // pwm value for 0 degrees(min 100), 90 degrees and 180 degrees (max 700)
       ServoWrite(0);
       delay(250);
     }
     Serial.println(F("Is the servo now facing (exactly) 90 degrees left (dir. 0 deg)? {y/n]"));
   }

   ServoWrite(180);
   delay(250);
   Serial.println(F("Is the servo facing 90 degrees right (dir. 180 deg)? {y/n]"));
   while(!getReply())
   {
     Serial.println(F("Enter L for left R for right and S for stop [l/r/s]"));
     while(1)
     {
       int Reply = getIntReply();
       if ( Reply==1 ) Servo0 += 5;
       if ( Reply==2 ) Servo0 -= 5;
       if ( Reply==3 ) break;
       ServoInit(Servo0, Servo90, Servo180); // pwm value for 0 degrees(min 100), 90 degrees and 180 degrees (max 700)
       ServoWrite(180);
       delay(250);
     }
     Serial.println(F("Is the servo now facing (exactly) 90 degrees right (dir. 180 deg)? {y/n]"));
   }
   
   Serial.println(F("Please write down:"));
   Serial.print(F("Servo   0 degrees value: "));
   Serial.println(Servo0);
   Serial.print(F("Servo  90 degrees value: "));
   Serial.println(Servo90);
   Serial.print(F("Servo 180 degrees value: "));
   Serial.println(Servo180);
   Serial.println("");

   ServoWrite(90);
   delay(250);

 
   Serial.println(F("That is all!"));
   Serial.println(F("Have fun programming!"));
   Serial.println(F("For the ChallengeBotHelloWorld program you need to set the serial monitor speed to 115200"));

   while(1);     
 
}

//-----------------------------------------------------------------------------------------
void loop() 
{
  
  
  
}

//-----------------------------------------------------------------------------------------
boolean getReply()
{  

  char AnswerChar;

  while(Serial.available()) Serial.read(); //clear buffer

  while(!Serial.available()); // wait for reply
  while(1)
  {
    while(Serial.available())
    {
      AnswerChar = Serial.read();
      if (AnswerChar == 'y' or AnswerChar == 'Y') return true;
      if (AnswerChar == 'n' or AnswerChar == 'N') return false;
    }
  }
}

//-----------------------------------------------------------------------------------------
int getIntReply()
{  

  char AnswerChar;
  int i;

  while(Serial.available()) Serial.read(); //clear buffer

  while(!Serial.available()); // wait for reply
  while(1)
  {
    while(Serial.available())
    {
      AnswerChar = Serial.read();
      if (AnswerChar == 'l' or AnswerChar == 'L') return 1;
      if (AnswerChar == 'r' or AnswerChar == 'R') return 2;
      if (AnswerChar == 's' or AnswerChar == 'S') return 3;
    }
  }
}



