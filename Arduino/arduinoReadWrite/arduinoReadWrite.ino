#include <Servo.h>
#include <Arduino.h>
#include <math.h>             //used for arctan function to get angle to blocks
#include "movement.h"
#include "distance.h"
#include "arm.h"

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

void setup() {
  // put your setup code here, to run once:
  //----------Movement setup----------
  pinMode(Pulse_FL, OUTPUT);
  pinMode(Dir_FL, OUTPUT);
  pinMode(Pulse_FR, OUTPUT);
  pinMode(Dir_FR, OUTPUT);
  pinMode(Pulse_BL, OUTPUT);
  pinMode(Dir_BL, OUTPUT);
  pinMode(Pulse_BR, OUTPUT);
  pinMode(Dir_BR, OUTPUT);
  digitalWrite(Dir_FL, HIGH);
  digitalWrite(Dir_FR, LOW);
  digitalWrite(Dir_BL, HIGH);
  digitalWrite(Dir_BR, LOW);
  
  //----------Arm Setup----------
  arm.attach(9);  // attaches the servo on pin 9 to the servo object arm
  pincer.attach(10);  // attaches the servo on pin 10 to the servo object pincer
  arm.write(inAngle);
  pincer.write(inAngle);
  
  //----------Serial Setup----------
  Serial.begin(9600);
}

void loop() {
  delay(3000);
  if(testCondition){
    findBlock(false);
    testCondition = false;
  }
//    pickup();
//    deposit();
  
  /*
  delay(1000);
  
  //testing
  //linear(10);
  double distance = lowSensor();
  logVal("Distance: ", distance);
//  if(distance > 20){
//    Serial.println("object too far");
//    rotate(-10);
//  }else if(distance > distanceFromArmToBlock){
//    logVal("Object detected!", "");
//    Serial.println("Moving to object");
//    double dtt = distance - distanceFromArmToBlock; //distance to travel
//    int distanceSteps = findSteps(dtt, "distance");
//    linear(distanceSteps);
//    updateLocation(0, stepsToDistance(distanceSteps));
//  }else{
//    Serial.println("Picking up object");
//    pickup();
//    deposit();
//  }
  
  /*
  if(root == "pi"){
    receiveData();
  }
  else{
    //travel to all locations
    for(int x = 0; x < 6; x++){
      findPath(blockX[x], blockY[x]);
    }
    //travels to 2,1 from 4,4
    //findPath(blockX[2], blockY[2]);
    //sendData(accum++);
  }
  */
}
/*
//prints value to serial monitor
void logVal(String msg, double val){
  Serial.println(msg + String(val));
}

void logVal(String msg, String val){
  Serial.println(msg + val);
}
*/

//distance: distance to object
bool checkIfObstacle(double distance){      // these currently call lowSensor, they need to call the High right sensor
  double a, b, c, theta;
  a = distance;
  b = 8.211;
  c = sqrt(sq(a)+sq(b));
  theta = asin(b/c);
  int steps = findSteps(-theta, "angle");
  rotate(steps);
  double dist = highRightSensor();
  if (dist <= 30){
    for (int i = 0; i < 19; i++){         // might not need this loop at all
      dist = dist + highRightSensor();  
    }
    dist = dist/20;
    return true;
  }
  else{
    return false;
  }
}


String coordToString(int x, int y){
  return String(x) + ", " + String(y); 
}

double degToRad(int deg){
  return ((double) deg)/180*M_PI;
}

//finds if doubles are withing a threshold of each other
bool equal(double val, double newVal){
  if(abs(val - newVal) < .0005){ return true; }
  else{ return false; }
}

//find errors & make corrections if errors aren't within some tolerance (angle version)
/*double errorA(double angle, double trudangle){    
  double errorA = angle - trudangle;                                        
  // need to decide on a tolerance
  if(abs(errorA) >= 5){
    int steps = findSteps( errorA, "angle");
    double dangle = rotate(steps);   // may need to check the error again, but I think that wouldn't be necessary depending on the tolerance    
    double del = errorA - dangle;
    curAngle = curAngle - del;
    return (trudangle + dangle);
   }
  else{
    curAngle = curAngle - errorA;
    return trudangle;
  }
}*/

// distance version
/*double errorD(double dist, double trudist){
   double errorD = dist - trudist;    // might want to move the current coordinate updates into this function
   if(abs(errorD) >= 5){              // not sure though
     int steps = findSteps( errorD, "distance");
     double deldist = linear(steps);
     return deldist;
   }
   else {
    return trudist; 
   }
}*/

//find angle from current location to coordinate. 0 is north
double findAngle(int x, int y){
  double delX = (x-currentCoord[0]);  //change in x
  double delY = (y-currentCoord[1]);  //change in y
  if(equal(delX, 0)){
    if(delY > 0){ return 0; }
    else{ return 180; } 
  }
  else if(equal(delY, 0)){ 
    if(delX > 0){ return 90; }
    else{ return -90; }
  }
  else{
    double angle = atan2( (x-currentCoord[0]), (y-currentCoord[1]) );
    angle=angle*180/3.141592653;  //radians => degrees
    return angle;
  }
}
// function to find block, and return the distance & update the angle.
double findBlock(boolean NE){
  boolean found = false;          // not currently using this; may take it out, or make use of it.
  boolean firstPass = false;
  boolean secondPass = false;
  int j = 0;
  int steps;
  int aSteps;
  double distance;
  double deldist;
  double dAngle;
  
  
  if (NE != true) {
    dAngle = helper_rotate(curAngle, -90);
    aSteps = findSteps(dAngle,"angle");
    rotate(aSteps);
    updateLocation(-90, 0);
  }
  else{
    dAngle = helper_rotate(curAngle, 90);
    aSteps = findSteps(dAngle,"angle");
    rotate(aSteps);
    updateLocation(90, 0);
  }
  
  if (firstPass != true){
      while(j <= 90){                  // 1 degree is 16.2 steps, so 90*16.2/16 = 91 remainder 2
        distance = lowSensor();    // so, need to do 2 extra steps 
        double newAngle;
        if (distance > 30){
          if (j == 89){
            rotate(-2);
          }
          rotate(-16);
          j++;
        }
        else{
          for(int i=0; i<19; i++){
            distance = distance + lowSensor();
          }
          distance = distance/20;
          bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
          if (obstacleCheck = false){  
            double dtt = distance - distanceFromArmToBlock; //distance to travel
            if(j<89){
              newAngle = curAngle + stepsToAngle(j*(-16));
            }
            else{
              newAngle = curAngle + stepsToAngle((j*(-16))-2);
            }
            updateLocation(newAngle, 0);
            return 10*dtt;  
          }
        }
      }
        double newAngle = curAngle + stepsToAngle((j*(-16))-2);
        updateLocation(newAngle, 0);
        firstPass = true;
  }
  
  //  ------- For 2nd and 3rd pass:
  //  ------- Need some type of test for when we are on a square next to the edge, to limit how far we go to 6 inches (or 12 iterations of 1/2 inch)
  //  ------- will need a separate test for each of the 2nd and third pass, something like the NE bool.
  //  ------- at the moment this is not being taken into account.
  //  ------- may want to check with the front low sensor again after turning.
  else if(firstPass == true && secondPass != true){
    if (NE != true) {
      dAngle = helper_rotate(curAngle, -90);
      aSteps = findSteps(dAngle,"angle");
      rotate(aSteps);
      updateLocation(-90, 0);
    }
    else{
      dAngle = helper_rotate(curAngle, 90);
      aSteps = findSteps(dAngle,"angle");
      rotate(aSteps);
      updateLocation(90, 0);
  }
      while( j<= 23){
         distance = lowSensor();      // this will need to be the low sensor on the left side of the robot
         if (distance > 30){
             steps = findSteps(12.7, "distance");
             linear(steps);
             j++;
         }
         else{
           aSteps = findSteps(-90, "angle");
           rotate(aSteps);
           distance = lowSensor();
           for(int i=0; i< 19; i++){
              distance = distance + lowSensor();  // this will need to be the low sensor on the center of the robot
           }
           distance = distance/20;  
           bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
           if (obstacleCheck = false){
             double dtt = distance - distanceFromArmToBlock; //distance to travel
             deldist = stepsToDistance(j*steps);
             updateLocation(curAngle-90, deldist);
             return 10*dtt;
           }
           else{
            aSteps = findSteps(90, "angle");
            rotate(aSteps);
           }
        }
      }
      deldist = -stepsToDistance(j*steps);
      steps = findSteps(deldist, "distance");
      linear(steps);
      updateLocation(curAngle, deldist);
      secondPass = true;
    }
    
  else{
    if (NE != true){
      dAngle = helper_rotate(curAngle, -180);
      aSteps = findSteps(dAngle, "angle");
      rotate(aSteps);
      updateLocation(-180, 0);
    }
    else{
      dAngle = helper_rotate(curAngle, 0);
      aSteps = findSteps(dAngle, "angle");
      rotate(aSteps);
      updateLocation(0, 0);
    }
    while( j<= 23){
         distance = lowSensor();      // this will need to be the low sensor on the right side of the robot
         if (distance > 30){
             steps = findSteps(12.7, "distance");
             linear(steps);
             j++;
         }
         else{
           aSteps = findSteps(90, "angle");
           rotate(aSteps);
           distance = lowSensor();
           for(int i=0; i< 19; i++){
              distance = distance + lowSensor();  // this will need to be the low sensor on the center of the robot
           }
           distance = distance/20;  
           bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
           if (obstacleCheck = false){  
             double dtt = distance - distanceFromArmToBlock; //distance to travel
             deldist = stepsToDistance(j*steps);
             updateLocation(curAngle+90, deldist);
             return 10*dtt;
           }
           else{
            aSteps = findSteps(-90, "angle");
            rotate(aSteps);
           }
        }
      }      // this next part *shouldn't* happen
      deldist = -stepsToDistance(j*steps);
      steps = findSteps(deldist, "distance");
      linear(steps);
      updateLocation(curAngle, deldist);
  }
}


//finds distance to travel
double findDistance(double x1, double x2, double y1, double y2){
  double distance = -1;
  double sqval = sq((x2-x1)) + sq((y2-y1));
  if(sqval > 0){ distance = sqrt(sqval); }
  distance = distance * 304.8;      //convert distance to millimeters
  return (distance);
}

//finds path to travel to point (x,y) from currentCoord.
//currently finds straight line
void findPath(int x, int y){
//  logVal("Current x-val: ", currentCoord[0]);
//  logVal("Current y-val: ", currentCoord[1]);
//  logVal("Target location: ", coordToString(x, y));
  double targetAngle = findAngle(x, y);
  double dAngle   = helper_rotate(curAngle, targetAngle); //change in angle
  double distance = findDistance(currentCoord[0], x, currentCoord[1], y);
  //logVal("distance", distance/304.8);
  int    dSteps   = findSteps(distance, "distance");
  int    aSteps   = findSteps(dAngle,   "angle"   );
  
  double trueDistance = stepsToDistance(dSteps);
  double trueAngle    = stepsToAngle(aSteps);

//  logVal("trueAngle", trueAngle);
//  logVal("Steps: ", aSteps);
  //logVal("trueDistance", trueDistance/304.8);
  //run path with true values
  runPath(aSteps, dSteps);   //travels determined distance
  updateLocation(trueAngle, trueDistance);
  delay(7000);
}

//moves robot to new angle and moves distance
void runPath(int aSteps, int dSteps){
  rotate(aSteps); //rotate delta angle 
  linear(dSteps); //travel distance in straight line
}

void updateLocation(double trueAngle, double trueDistance){
  curAngle         = trueAngle;
  double rad       = degToRad(trueAngle);           //angle in radians
  //new location, convert distance to block location
  currentCoord[0] += sin(rad) * trueDistance/304.8;
  currentCoord[1] += cos(rad) * trueDistance/304.8;
//  String display = String(currentCoord[0]) + ", " + String(currentCoord[1]); 
//  logVal("New location: ", display);
}

//gets data from raspberry pi
void receiveData(){
  if (Serial.available() > 0) { // is a character available?
    msg = Serial.read();        // get the character
    Serial.println(msg);
  }
}

void sendData(int val){
  Serial.println(val);
  root = "pi"; //sets pi to active
}
