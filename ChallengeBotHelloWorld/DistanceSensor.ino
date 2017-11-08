/*
  DistanceSensor Module
 
 Ultrasonic HC-SR04 distance sensor
 Uses a trigger pulse to send a ultrasone burst.
 Then waits for the echo to return 
 The duration divided by 2 and the result devided by 29.14 gives the distance in cm.
 (29.14 = time in microsecconds it takes sounds to travel a distance of 1cm)

 After the measurement we need some time for echo's to decay.
 If a next measurement is requested within 10 ms. the previous value is returned
  
*/

//-----------------------------------------------------------------------------------------------
// Hardware declaration

#define DISTANCESENSECHO  6 // distance sensor echo pin
#define DISTANCESENSTRIG  7 // distance sensor trigger pin
 
//-----------------------------------------------------------------------------------------------
// Inititialsiation 

void distanceSensorInit() 
{
  pinMode(DISTANCESENSTRIG, OUTPUT); // set trigger pin as output
  pinMode(DISTANCESENSECHO, INPUT);  // set echo pin as input

  digitalWrite(DISTANCESENSTRIG, LOW); // set trigger pin low (trigger is high)  
}
 
//-----------------------------------------------------------------------------------------------
// Read sensor

int distanceSensorRead() 
{
  #define ECHODELAY 30 //
  
  static long measureTime = 0;
  static int lastReturnValue = 0;
  long duration = 0;
  
  if( (millis()-measureTime) > ECHODELAY ) // if not to soon after the previous measurment to avoid echos from before
  {
    measureTime = millis(); // note the measurment time
    digitalWrite(DISTANCESENSTRIG, HIGH); // trigger pin high
    delayMicroseconds(10); // 10 microseccond  delay
    digitalWrite(DISTANCESENSTRIG, LOW); // trigger pin low 
    duration = pulseIn(DISTANCESENSECHO, HIGH, 12000); // measure the echo time in microsecconds; limit to approx 200 cm
    lastReturnValue = int (float(duration / 2) / 29.14); // calculate distance in cm
  }
  return lastReturnValue; // return the measurement (or the previous one if requested too soon )
  
}

//-----------------------------------------------------------------------------------------------
// Obstacle warding

boolean distanceSensorCheckObstacle(int obstacleDistWarning)
{
   int distanceMeasured;
   
   distanceMeasured = distanceSensorRead();
   if( distanceMeasured > obstacleDistWarning || distanceMeasured == 0 )
     return false;

   Serial.print( "Obstacle: "); // show obstacle detected
   Serial.print( distanceMeasured );
   Serial.println(" cm.");
   return true;
}

// end module
