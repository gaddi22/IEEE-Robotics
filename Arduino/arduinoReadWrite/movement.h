
//----------------State Manager Variables---------------
char   msg            = 0;          //read from pi
String root           = "ard";      //sets arduino to active
int    accum          = 0;          //number of blocks picked up
double curAngle       = 0;          //current Degees Robot is facing 
double currentCoord[] = {4, 4};     //location of robot
double blockX[] = {3,5,2,7,2,7};    //blocks' Xcoordinates
double blockY[] = {4,5,1,3,0,6};    //blocks' Ycoordinates
int    distanceFromArmToBlock = 11; //cm, minimum distance to pick up block
bool   testCondition = true;        //used to test a single iteration


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

double degToRad(int deg){
  return ((double) deg)/180*M_PI;
}

//converts angle or distance to a number of corresponding steps
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

void updateLocation(double trueAngle, double trueDistance){
  curAngle         = curAngle + trueAngle;
  double rad       = degToRad(trueAngle);           //angle in radians
  //new location, convert distance to block location
  currentCoord[0] += sin(rad) * trueDistance/304.8;
  currentCoord[1] += cos(rad) * trueDistance/304.8;
//  String display = String(currentCoord[0]) + ", " + String(currentCoord[1]); 
//  logVal("New location: ", display);
}

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
  double truDist = stepsToDistance(steps);
  updateLocation(0, truDist);
}

//steps: number of steps equals to dAngle
void rotate(int steps)  // callable function for rotation
{                       // number of steps may need tweaking, depends on the weight distribution; 
 //                          which will have to wait until the robot is fully built
 if (steps <= 50){
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
      delayMicroseconds(1000);
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
      delayMicroseconds(1000);
    }
    digitalWrite(Dir_FL, HIGH);
    digitalWrite(Dir_BL, HIGH);
  }
 }
 else{
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
  double truAngle = stepsToAngle(steps);
  updateLocation(truAngle, 0);  
}
