#include <Servo.h>
#include <Arduino.h>
#include <math.h>             //used for arctan function to get angle to blocks
#include "movement.h"
#include "distance.h"
#include "arm.h"
#include "color.h"


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

  //----------Color Senser----------
  tcs.begin();

  //----------Magnetometer----------
  //mag.begin();
  
  //----------Serial Setup----------
  Serial.begin(9600);
  delay(2500);
}

void loop() {


  delay(3000);
  int dSteps = findSteps(1219, "distance");
  linear(dSteps);
  linear(-dSteps);
  int aSteps = findSteps(360.0, "angle");
  rotate(aSteps);
//  findMotherShip();
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
  double distanceToTravel = 1;      //1 foot
  int    directionClassifier = 1;  //NE or not? 1 means not NE
  double distance = 0;  //distance travelled
  if (angle > 0 && angle < 90){
    directionClassifier = -1;
  }

  angleToTurn *= directionClassifier;

  //---------------1st Pass---------------
  turnTo(angleToTurn); //turns until it reaches angleToTurn
  double lowD = lowSensor(); 
  found = lowD < 14;
  double newAngle = angleToTurn;
  while ( newAngle > -180 && !found){
    //Serial.println("1");
    int steps = findSteps(1, "angle");
    rotate(-steps);
    double angleRotated = stepsToAngle(steps);
    newAngle -= angleRotated;
    lowD = lowSensor(); 
    found = lowD < 14;
  }
  

  if(!found){//---------------2nd Pass---------------
    turnTo(angleToTurn);
    while(distance < distanceToTravel && !found){
      Serial.println("2");
      linear(3); //move 3 steps forwards
      //update distance travelled
      //found = lowLeftSensor < 20;
    }
  }else{ 
    angleToBlock    = curAngle;    //in deg
    //distanceToBlock = lowSensor(); //in cm
  }
  
  if(!found){ //---------------3rd Pass---------------
    Serial.println("3");
    linear(-distance);
    distance = 0;
    turnTo(-180);
    while(distance < distanceToTravel && !found){
      linear(3); //move 3 steps forwards
      //update distance travelled
      //found = lowRightSensor() < 20;
      if(found){
        angleToBlock = -90;
      //  distanceToBlock = lowRightSensor();
      }
    }
  }else{
    angleToBlock    = -180;    //in deg
    //distanceToBlock = lowLeftSensor(); //in cm
  }
  if(found){
   Serial.println("Found"); 
  }else{
    Serial.println("Not Found"); 
    return 0; //false, not found
  }
  return 1;
}



//finds distance to travel
double findDistance(double x1, double x2, double y1, double y2){
  double distance = -1;
  double sqval = sq((x2-x1)) + sq((y2-y1));
  if(sqval > 0){ distance = sqrt(sqval); }
  distance = distance * 304.8;      //convert distance to millimeters
  return (distance);
}

// funtion to rotate up to 180 degrees (90 CCW, then 90 Cw), and find the lowest distance from the lowSensor
// also takes in optional boolean for mothership,and optional double for the change in angle, these default to false & -1 respectively
void rotateScan(double distance, double delAngle = -1){
  double lowDistAngle;
  double lowDist = distance;
  double returnAngle = curAngle;
  int steps = findSteps(delAngle, "angle");           // this might have to change for accuracy reasons, not sure right now
  for(int angle = 0; angle < 90; angle++){      // turn 90 degrees CCW, scan after each degree
    rotate(steps);
    double dist2 = lowSensor();
    if( dist2 < lowDist){                      // if the new scan is lower than the lowest scan
      lowDist = dist2;                         // set the lowest scan to the current scan
      lowDistAngle = curAngle;                  // store the angle of the lowest scan
    }
    else if(dist2 > lowDist){                  // if the distance is growing, break out & go to the next function.
      break;
    }
  }
  turnTo(returnAngle);                          // go back to the original angle
  for(int angle = 0; angle < 90; angle++){      // loop to go 90 degrees CW, still scanning after each degree
    rotate(-steps);                             // the 90 degrees is assumming delAngle is not specified, it will be proportionally more or less depending on specification
    double dist2 = lowSensor();
    if(dist2 < lowDist){
      lowDist = dist2;
      lowDistAngle = curAngle;                  // same stuff
    }
    else if(dist2 > lowDist){
      break;
    }
  }
  if (lowDist < 14){                           // rotate to the angle that had the lowest distance
    turnTo(lowDistAngle);
  }
}

// function to find and store mothership data.
void findMotherShip(){
  Serial.println("0");
  bool found = false;
//  double motherShipAngle, motherShipX, motherShipY;
  double pathX[] = {2, 6};
  double pathY[] = {4, 4};    // set points for checking, if we don't find it on the first 360
  bool isgreen = false;                          
  int steps = findSteps(-15.0, "angle");
  for (int i = 0; i <24; i++){            // do 360 degree check on the starting point.
    isgreen = isGreenPresent();
    if( isgreen == true){                 // if green is present in the current direction, break out of the loop
      found = true;
      Serial.println("1");
      break;
    }
    rotate(steps);                        // rotate 10 degrees
  }
  if (found != true){                     // if the mothership isn't found, 
    turnTo(0.0);
    for (int i = 0; i <= 1; i++){         //  go to the next point and do a scan there
      double targetAngle = findAngle(pathX[i], pathY[i]);
      turnTo(targetAngle);
      double distance = findDistance(currentCoord[0], pathX[i], currentCoord[1], pathY[i]);
      int dSteps = findSteps(distance, "distance");
      linear(dSteps);
      turnTo(curAngle + 120.0);
      for (int i = 0; i < 16; i++){       // there might be a cleaner way of doing this, but the idea
                                          // is to do two seperate 120 degree scans, checking every 10 degrees
        isgreen = isGreenPresent();
        if( isgreen == true){
          found = true;                   // if we find it, set found to true, and break out of the loop
          Serial.println("2");
          break;
        }
        rotate(steps);                    // first one is CCW
      }
      if( found == true){                 // if we found the mothership, break out of the loop
        Serial.println("4");
        break;                            // this may seem redundant, but we have to do this twice since there are 2 for loops
      }
    }
  }
  else{
    double dtt= 100;                      // dtt = distance to target
    while(dtt > 14){
  // need to step towards the source of the green light, in increments of our sensor range; 14cm, or 140 mm
      double distance = 140.0;
      int dSteps = findSteps(distance, "distance");
  // sensor scan of the lowSensor, and the highSensor   high sensor isn't currently set up, but will be soon(TM)
      dtt = lowSensor();
      if (dtt > 14){
        linear(dSteps);
      }
      else{                                   // need to find out if we are pointed at a corner
        rotateScan(dtt);
        double returnAngle = curAngle;
        steps = findSteps(-20.0, "angle");
        rotate(steps);
        bool isgreen1 = isGreenPresent();
        turnTo(returnAngle);
        rotate(-steps);
        bool isgreen2 = isGreenPresent();
        turnTo(returnAngle);
        if(isgreen1 == true && isgreen2 == true){
          return;
        }
        else {
          double dist = lowSensor();                       // dist from sensor to corner
          double x = 142.5;                                // dist from sensor to edge of wheel
          double theta = asin(x/(sqrt(sq(x)+sq(dist))));   // angle 
          steps = findSteps(theta, "angle");               // steps for rotation
          dSteps = findSteps(108.24, "distance");          // steps for 1/2 the distance of the mothership
          if (isgreen1 == true){
            rotate(-steps);                                // CCW rotation
            linear(dSteps); 
            steps = findSteps(90.0, "angle");                // rotate 90 degrees back
            rotate(steps);         
          }
          else{
            rotate(steps);                                 // CW rotation
            linear(dSteps);
            steps = findSteps(-90.0, "angle");
            rotate(steps);                                 // rotate 90 degrees back
          }
          dtt = lowSensor();                               // take another measurement
          rotateScan(dtt);                                 // find orthagonal
          return;
        }
      }
    }
  }
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
/*
//receives data from pi and loads it into state variables
void receiveBlockData(){
  int qty = 0;
  bool x = true;
  int index = 0;
  bool runBit = true; //continue reading data

  while(runBit){
    if (Serial.available() > 0) { //check if character is available.
      char val = Serial.read();
      if(qty==0){ //received qty value
        qty = int(val)%48;
        blockQty = qty;        
      }else{ //received block location
        if(x){ //load x values
          blockX[index++] = int(val)%48;
          if(index >= qty){
            index = 0;
            x = false;
          }
        }else{ //load y values
          blockY[index++] = int(val)%48;
          if(index >= qty){
            runBit = false;
          }
        }
      }
    }
  }
}
*/


//moves robot to new angle and moves distance
void runPath(int aSteps, int dSteps){
  rotate(aSteps); //rotate delta angle 
  linear(dSteps); //travel distance in straight line
}

void turnTo(double targetAngle){
  double delAngle = helper_rotate(curAngle, targetAngle);
  int steps = findSteps(delAngle, "angle");
  rotate(steps);
}

// function to zero in on a block, to be run either in or after findBlock function
// this attempts to aim at the center of a block, & return the shortest distance from the robot to the block.
// hopefully to deal with angled blocks
double zeroInOnBlock(double distance){
  double CCWAngle, CWAngle;
  double returnAngle = curAngle;
  int steps = findSteps(-.5, "angle");           // this might have to change for accuracy reasons, not sure right now
  for(int angle = 0; angle < 10; angle++){      // turn 90 degrees CCW, scan after each degree
    rotate(steps);
    double dist2 = lowSensor();
    if( dist2 < distance){                      // if the new reading is lower than the lowest reading
      distance = dist2;                         // set the lowest reading to the current reading
    }
    else if(dist2 > 14){                  // if the distance is growing, break out & go to the next function.
      break;
    }
    CCWAngle = curAngle;
  }
  turnTo(returnAngle);                          // go back to the original angle
  for(int angle = 0; angle < 10; angle++){      // loop to go 90 degrees CW, still scanning after each degree
    rotate(-steps);
    double dist2 = lowSensor();
    if(dist2 < distance){
      distance = dist2;                         // same stuff
    }
    else if(dist2 > 14){
      break;
    }
    CWAngle = curAngle;
  }
  double tarAngle = (CCWAngle + CWAngle)/2;
  turnTo(tarAngle);
  return distance;
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
