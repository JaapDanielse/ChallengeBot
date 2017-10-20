/*
    ChallengeBotHelloWorld (c)J.C. Daniëlse - October 2017

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



  V1.0   JnD & JpD 20-10-2017
         Original version based on  

*/

//-----------------------------------------------------------------------------------------
// Motor Direction Values

// #define DEBUG //

 #define STOP     0 // 
 #define FORWARD  1 //
 #define REVERSE  2 //

// Speed and Motor PWM Slow
 #define SLOW 450 // Speed in mm/sec for slow drive
 #define SLOWPWM 80 // PWM value for slow drive (experiment with setting for your robot)

// Speed and Motor PWM Fast
 #define FAST 800 // Speed in mm/sec for fast drive
 #define FASTPWM 255 // PWM value for fast drive ( max pwm )

 #define MAXLOOP 4 // maximum time back and forth

//-----------------------------------------------------------------------------------------
// Typedefs
 typedef void (*ISRCallBack)( byte, int, int ); // callback declaration


//-----------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200); // set up Serial library at 115200 bps
  Serial.println("Robot Hello World!");
 
  speedSensorInit( driveSpeedSensorCallback );
  distanceSensorInit();
  motorControlInit();
  ServoInit(120, 325, 590); // pwm value for 0 degrees min 100, 90 degrees and 180 degrees (max 700)
                            // allows to have the 90 degree position straight ahead. 

}


//-----------------------------------------------------------------------------------------
void loop() 
{

  static int action = 1;
  static int lastAction = 0; 
  static int loopCount = 0;
  static int setDistance = 0;
  static int SweepArray[61];
  static int ObstacleDirection = 0;
  static int ObstacleDistance = 0;
  static int ObstacleTurn = 0;
  int i=0;


//  while(1); 
 
  switch ( action )
  {
    case 1:
    { // init for drive
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action;
      speedSensorClear(); // clear counts
      setDistance = 700; // set distance for drive in mm.
      action = 2; // set next action
      break; // exit action
    }

    case 2:
    { // drive fast
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action; // remeber where we are
      if ( !driveStraight( FORWARD, setDistance ,FAST, 20 ) ) // drive: Fast , detect obstacles < 20cm )
      { 
        setDistance = setDistance - driveDistanceDone() + 300; // calculate the rest of the distance to go.
        action=3; // react to obstacle in action 10
        break; // exit action
      } // obstacle detected
      setDistance =300; // set distance for next action
      action = 3; // set next action
      break; // exit action
    }
    
    case 3:
    { // drive slow
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action; // remeber where we are
      if ( !driveStraight( FORWARD, setDistance ,SLOW, 5 )) // drive: slow , detect obstacles < 5cm )
      { // obstacle detected
        setDistance = setDistance - driveDistanceDone(); // calculate the rest of the distance to go.
        action=10; // react to obstacle in action 11
        break; // exit action
      }
      action = 4; // set next action
      break; // exit action
    }

    case 4:
    { // turn around
      Serial.print("action: ");
      Serial.println( action );
      lastAction = action; // remember where we are
      driveStop(); // stop
      delay(500); // wait half a second 

      ServoSweep( SweepArray );
      if (SweepAnalyse( SweepArray, 50, 3, &ObstacleDirection, &ObstacleDistance))
      {
        Serial.print ("Target: ");
        Serial.print (90 + ObstacleDirection);
        Serial.print (" degrees, distance:");
        Serial.println (ObstacleDistance/10);
        delay(500);
        ServoWrite(90);
      }
      
      driveTurn(180); // turn around
      driveStop(); // stop 
      loopCount++; // remember how many times we did this.
      action = 1; // set next action
      break; // exit action
    }

    case 10:
    { // an obstacle is detected: stop and wait for it to go away
      Serial.print("action: ");
      Serial.println( action );
      driveStop(); // stop
      while(distanceSensorCheckObstacle(10)) // check if obstacle has gone
        delay(250); // wait 1/4 second
      action = lastAction; // return to action we were doing.
      break;
    }


    default:
      break; 
  }

  if (loopCount >= MAXLOOP) while(1);
  
}

// end module
