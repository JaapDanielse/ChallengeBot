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

         
  V1.2   JpD & JnD 8-11-2017
         Interrupt on both spoke and slot index wheel, Improved turn and distance calculation, code cleanup and comments
         
  V1.1   JpD 21-10-2017
         Removed speed constraint.
         
  V1.0   JnD & JpD 20-10-2017
         Original version based on GPO RobotHelloWorld (adapted for different hardware, servo added)

*/

//-----------------------------------------------------------------------------------------
// Defines

// Debug switch
// #define DEBUG // un-comment this define if you want debug data (monitor 15200 baud)

// Motor defines
#define FORWARD  1  // Motor direction forward
#define REVERSE  2  // Motor direction reverse
#define SLOW 1      // Slow drive indicator
#define FAST 2      // Fast drive indicator
#define SLOWPWM 100 // PWM value for slow drive ( Take value from ChallengeBotTest )
#define FASTPWM 255 // PWM value for fast drive ( max pwm )

// Behaviour defines
#define MAXLOOP 4 // maximum time back and forth

//-----------------------------------------------------------------------------------------
// Typedefs
 typedef void (*ISRCallBack)( byte ); // interrupt service callback routine declaration

//-----------------------------------------------------------------------------------------
// Robot initialisation

void setup() 
{
  Serial.begin(115200); // set up Serial library at 115200 bps
  Serial.println("ChallengeBot Hello World!");
 
  speedSensorInit( driveSpeedSensorCallback );
  distanceSensorInit();
  motorControlInit();
  ServoInit(115, 320, 580); // pwm value for 0 degrees min 100, 90 degrees and 180 degrees (max 700)
                            // allows to have the 90 degree position straight ahead. 
                            // Take value from ChallengeBotTest

  while(!distanceSensorCheckObstacle(3)) // start when signalled
  {
    delay(100); // wait 1/10 second
  }

  delay(1000); 
}

//-----------------------------------------------------------------------------------------
// Main Loop, Robot behaviour

void loop() 
{

  static int loopCount = 0;
  static int currentAction = 1;
  static int lastAction = 0; 
  static int setDistance = 0;
  static int SweepArray[61];
  static int ObstacleDirection = 0;
  static int ObstacleDistance = 0;

  switch ( currentAction )
  {
    case 1:
    { // init for drive
      Serial.print("action: ");
      Serial.println( currentAction );
      lastAction = currentAction;
      speedSensorClear(); // clear counts
      setDistance = 700; // set distance for drive in mm.
      currentAction = 2; // set next action
      break; // exit action
    }

    case 2:
    { // drive fast
      Serial.print("action: ");
      Serial.println( currentAction );
      lastAction = currentAction; // remember where we are
      if ( !driveStraight( FORWARD, setDistance ,FAST, 20 ) ) // drive: Fast , detect obstacles < 20cm )
      { // onstacle detected
        setDistance = setDistance - driveDistanceDone() + 300; // calculate the rest of the distance to go.
        currentAction=3; // react to obstacle in action 3
        break; // exit action
      } 
      setDistance = 300; // set distance for next action
      currentAction = 3; // set next action
      break; // exit action
    }
    
    case 3:
    { // drive slow
      Serial.print("action: ");
      Serial.println( currentAction );
      lastAction = currentAction; // remember where we are
      if ( !driveStraight( FORWARD, setDistance ,SLOW, 5 )) // drive: slow , detect obstacles < 5cm )
      { // obstacle detected
        setDistance = setDistance - driveDistanceDone(); // calculate the rest of the distance to go.
        currentAction=10; // react to obstacle in action 10
        break; // exit action
      }
      currentAction = 4; // set next action
      break; // exit action
    }

    case 4:
    { // turn around
      Serial.print("action: ");
      Serial.println( currentAction );
      lastAction = currentAction; // remember where we are
      driveStop(); // stop
      delay(500); // wait half a second 

      ServoSweep( SweepArray ); // look around
      if (SweepAnalysis( SweepArray, 50, 3, &ObstacleDirection, &ObstacleDistance))
      { // small object within 50 cm.
        Serial.print ("Target: ");
        Serial.print (90 + ObstacleDirection);
        Serial.print (" degrees, distance:");
        Serial.println (ObstacleDistance/10);
        delay(500); // wait half a secons
        ServoWrite(90); // look straight ahead

        driveTurn(ObstacleDirection); // turn the robot in the obstacle direction
        driveStop(); // stop 
        delay(1000); // wait obe second
        driveTurn(-ObstacleDirection); // turn back 
        driveStop(); // stop 
        delay(1000); // wait one second
      }
      
      driveTurn(180); // turn around
      driveStop(); // stop 
      loopCount++; // remember how many times we did this.
      currentAction = 1; // set next action
      break; // exit action
    }

    case 10:
    { // an obstacle is detected: stop and wait for it to go away
      Serial.print("action: ");
      Serial.println( currentAction );
      driveStop(); // stop
      while(distanceSensorCheckObstacle(10)) // check if obstacle has gone
      { // obstacle still there
        delay(250); // wait 1/4 second
      }
      currentAction = lastAction; // return to action we were doing.
      break;
    }

    default:
    { // no default action
      break; 
    }
  } // end switch

  if (loopCount >= MAXLOOP) while(1); // when done hold here.
}

// end module
