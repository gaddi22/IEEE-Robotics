#include <Servo.h>


// setup for stepper pins & declaring servos
int Pulse_FL = 2;
int Dir_FL = 3;
int Pulse_FR = 4;
int Dir_FR = 5;
int Pulse_BL = 6;
int Dir_BL = 7;
int Pulse_BR = 8;
int Dir_BR  = 11;

int angle;
int dist;
int i;

double angleConversionFactor    = 2.025*8;
double distanceConversionFactor = 1.064 *4;

void linear(int steps) // callable function for forwards and backwards movement
{
  int j = 0; 
  unsigned long currTime;                 // accelerates to 2.083 rev/s, or up to half the steps
  if (steps >=0){                         // decelerates for the same amount; extra steps are done at a constant speed
   currTime = millis();
   while (millis() - currTime < 1000){
    if(j< (steps/2)){
      digitalWrite(Pulse_FL, HIGH);
      digitalWrite(Pulse_FR, HIGH);
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
      delayMicroseconds(1500-(1.5*(millis()-currTime)));
      j++;
    }
    else{
      break;
    }
   }
   for(int i =0; i < (steps - (2*j)); i++){
     digitalWrite(Pulse_FL, HIGH);
     digitalWrite(Pulse_FR, HIGH);
     digitalWrite(Pulse_BL, HIGH);
     digitalWrite(Pulse_BR, HIGH);
     delayMicroseconds(200);
     digitalWrite(Pulse_FL, LOW);
     digitalWrite(Pulse_FR, LOW);
     digitalWrite(Pulse_BL, LOW);
     digitalWrite(Pulse_BR, LOW);
     delayMicroseconds(400);
   }
   currTime = millis();
   for (int i = 0; i < j; i++){
     digitalWrite(Pulse_FL, HIGH);
     digitalWrite(Pulse_FR, HIGH);
     digitalWrite(Pulse_BL, HIGH);
     digitalWrite(Pulse_BR, HIGH);
     delayMicroseconds(200);
     digitalWrite(Pulse_FL, LOW);
     digitalWrite(Pulse_FR, LOW);
     digitalWrite(Pulse_BL, LOW);
     digitalWrite(Pulse_BR, LOW);
     delayMicroseconds(400);
     delayMicroseconds(1.5*(millis() - currTime));
   }
  }
  else{
    digitalWrite(Dir_FL, LOW);
    digitalWrite(Dir_FR, HIGH);
    digitalWrite(Dir_BL, LOW);
    digitalWrite(Dir_BR, HIGH);
    currTime = millis();
    while (millis() - currTime < 1000){
      if(j< abs(steps/2)){
        digitalWrite(Pulse_FL, HIGH);
        digitalWrite(Pulse_FR, HIGH);
        digitalWrite(Pulse_BL, HIGH);
        digitalWrite(Pulse_BR, HIGH);
        delayMicroseconds(200);
        digitalWrite(Pulse_FL, LOW);
        digitalWrite(Pulse_FR, LOW);
        digitalWrite(Pulse_BL, LOW);
        digitalWrite(Pulse_BR, LOW);
        delayMicroseconds(400);
        delayMicroseconds(1500-(1.5*(millis()-currTime)));
        j++;
      }
      else{
        break;
      }
    }
    for(int i =0; i < (abs(steps) - (2*j)); i++){
      digitalWrite(Pulse_FL, HIGH);
      digitalWrite(Pulse_FR, HIGH);
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
    }
    currTime = millis();
    for (int i = 0; i < j; i++){
      digitalWrite(Pulse_FL, HIGH);
      digitalWrite(Pulse_FR, HIGH);
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
      delayMicroseconds(1.5*(millis() - currTime));
    }
    digitalWrite(Dir_FL, HIGH);
    digitalWrite(Dir_FR, LOW);
    digitalWrite(Dir_BL, HIGH);
    digitalWrite(Dir_BR, LOW);
  }
}

//steps: number of steps equals to dAngle
void rotate(int steps)  // callable function for rotation
{                       // number of steps may need tweaking, depends on the weight distribution; 
 //                          which will have to wait until the robot is fully built
  if (steps >= 0) 
  {
    digitalWrite(Dir_FR, HIGH);
    digitalWrite(Dir_BR, HIGH);
    for ( i = 1; i <= steps; ++i)          //speed at 2.083 rev/s tweaking the 2nd delay can tweak the speed
    {
      digitalWrite(Pulse_FL, HIGH);       //currently using 4 microsteps, this is being taken into account in
      digitalWrite(Pulse_FR, HIGH);       // findSteps function
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
    }
    digitalWrite(Dir_FR, LOW);
    digitalWrite(Dir_BR, LOW);   
  }
  else 
  {
    digitalWrite(Dir_FL, LOW);
    digitalWrite(Dir_BL, LOW);
    for(int i=0; i < abs(steps); i++){
      digitalWrite(Pulse_FL, HIGH);
      digitalWrite(Pulse_FR, HIGH);
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
    }
    digitalWrite(Dir_FL, HIGH);
    digitalWrite(Dir_BL, HIGH);
  }  
}

double helper_rotate(double olddeg, double newdeg){
  double deldeg = newdeg - olddeg;
  return deldeg;
}

double stepsToDistance(int steps){
  return steps/distanceConversionFactor;
}

double stepsToAngle(int steps){
  return steps/angleConversionFactor;
}

//converts angle or distance to a number of corresponding steps
//val: distance in mm if type = distance
int findSteps(double val, String type){
  int steps = 0;
  if(type == "distance"){
    //find distance step qty
    steps = (int)floor(val*distanceConversionFactor);
  }
  else if(type == "angle"){
    //find angle step qty
    steps = (int)round(val*angleConversionFactor);

  }  
  return steps;
}

//blockNum: block we are currently searching for
//loc:      location of block currently being searched for
bool findBlock(int blockNum){
  //values returned
  int distanceToBlock = 0;
  int angleToBlock    = 0;

  //Values to solve problem
  bool found = false;
  int  loc[] =  { blockX[blockNum], blockY[blockNum] };
  double angle = findAngle(loc[0], loc[1]); 
  double angleToTurn = -90;        //left of right
  double distanceToTravel = 1      //1 foot
  int    directionClassifier = 1;  //NE or not? 1 means not NE
  double distance = 0;  //distance travelled
  if (angle > 0 && angle < 90){
    directionClassifier = -1;
  }

  angleToTurn *= directionClassifier;

  //---------------1st Pass---------------
  turnTo(angleToTurn); //turns until it reaches angleToTurn
  while ( newAngle > -180 && !found){
    steps = findSteps(2, "angle");
    rotate(-steps);
    //update newAngle
    found = lowSensor < 20;
  }

  if(!found){//---------------2nd Pass---------------
    turnTo(angleToTurn);
    while(distance < distanceToTravel && !found){
      linear(3); //move 3 steps forwards
      //update distance travelled
      found = lowLeftSensor < 20;
    }
  }else{ 
    angleToBlock    = curAngle;    //in deg
    distanceToBlock = lowSensor(); //in cm
  }
  
  if(!found){ //---------------3rd Pass---------------
    linear(-distance);
    distance = 0;
    turnTo(-180);
    while(distance < distanceToTravel && !found){
      linear(3); //move 3 steps forwards
      //update distance travelled
      found = lowRightSensor() < 20;
      if(found){
        angleToBlock = -90;
        distanceToBlock = lowRightSensor();
      }
    }
  }else{
    angleToBlock    = -180;    //in deg
    distanceToBlock = lowLeftSensor(); //in cm
  }
  
  return found;
}