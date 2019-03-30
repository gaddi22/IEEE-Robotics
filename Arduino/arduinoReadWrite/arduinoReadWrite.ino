#include <Servo.h>
#include <Arduino.h>
#include <math.h>             //used for arctan function to get angle to blocks
#include "movement.h"
#include "distance.h"
#include "arm.h"



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
  int delSteps = 0;               // j renamed to delSteps
  int steps;
  int aSteps;
  double distance;
  double deldist;
  double dAngle;
  double distanceToTravel;
  double newAngle;
  
  
  if (NE != true) {
    dAngle = helper_rotate(curAngle, -90);
    aSteps = findSteps(dAngle,"angle");
    rotate(aSteps);
    newAngle = stepsToAngle(aSteps);
    updateLocation(newAngle, 0);
  }
  else{
    dAngle = helper_rotate(curAngle, 90);
    aSteps = findSteps(dAngle,"angle");
    rotate(aSteps);
    newAngle = stepsToAngle(aSteps);
    updateLocation(newAngle, 0);
  }
  while(delSteps < findSteps(90, "angle"))      // 90 degrees is 1458 steps, doing this in increments of 9, because 1458 is divisible by 9.
  {                                             //    next possible increment is 18, which is over 1 degree, but not sure how much we care about going in increments of less than 1 degree.
    distance = lowSensor();                     //  could also use the constant 1458 instead of findSteps(90,"angle"), but this way gives it context for now
    if (distance > 30)
    {
      rotate(-9);
      delSteps = delSteps + 9;
    }
    else
    {
      for(int i=0; i<19; i++)
      {
        distance = distance + lowSensor();
      }
      distance = distance/20;
      bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
      if (obstacleCheck == false)
      {  
        distanceToTravel = distance - distanceFromArmToBlock; //distance to travel
        newAngle = stepsToAngle(-delSteps);
        updateLocation(newAngle, 0);
        found = true;
        break;
      }
    }
  }
  newAngle = stepsToAngle(-delSteps);
  updateLocation(newAngle, 0);


  
  //  ------- For 2nd and 3rd pass:
  //  ------- Need some type of test for when we are on a square next to the edge, to limit how far we go to 6 inches (or 12 iterations of 1/2 inch)
  //  ------- will need a separate test for each of the 2nd and third pass, something like the NE bool.
  //  ------- at the moment this is not being taken into account.
  //  ------- may want to check with the front low sensor again after turning.
  if (found == false)
  {
    if (NE != true) 
    {
      dAngle = helper_rotate(curAngle, -90);
      aSteps = findSteps(dAngle,"angle");
      rotate(aSteps);
      newAngle = stepsToAngle(aSteps);
      updateLocation(newAngle, 0);
    }
    else
    {
      dAngle = helper_rotate(curAngle, 90);
      aSteps = findSteps(dAngle,"angle");
      rotate(aSteps);
      newAngle = stepsToAngle(aSteps);
      updateLocation(newAngle, 0);
    }
    delSteps = 0;
    while( delSteps < 1296)        // delSteps is the step counter, for this one, while it is less than the # of steps for 1 foot
    {                              //   this value is 1297.2288; but we're flooring it, & it is an int, so it'll be 1297.
                                   //   subtracting 1 gives a clean iteration of 54; so new 
      distance = lowSensor();      // this will need to be the low sensor on the left side of the robot (which we don't have set up atm)
      if (distance > 30)
      {
        linear(54);
        delSteps = delSteps + 54;
        deldist = stepsToDistance(54);
        updateLocation(0, deldist);
      }
      else
      {
        aSteps = findSteps(-90, "angle");
        rotate(aSteps);
        distance = lowSensor();
        for(int i=0; i< 19; i++)
        {
           distance = distance + lowSensor();  // this will need to be the low sensor on the center of the robot
        }
        distance = distance/20;  
        bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
        if (obstacleCheck == false)
        {
          distanceToTravel = distance - distanceFromArmToBlock; //distance to travel
          newAngle = stepsToAngle(aSteps);
          updateLocation(newAngle, 0);
          found = true;
          break;
        }
        else
        {
          aSteps = findSteps(90, "angle");
          rotate(aSteps);
        }
      }
    }
  }
  if (found == false)
  {
    deldist = stepsToDistance(-delSteps);
    steps = findSteps(deldist, "distance");
    linear(steps);
    updateLocation(0, deldist);
      
    if (NE != true)
    {
      dAngle = helper_rotate(curAngle, -180);
      aSteps = findSteps(dAngle, "angle");
      rotate(aSteps);
      newAngle = stepsToAngle(aSteps);
      updateLocation(newAngle, 0);
    }
    else
    {
      dAngle = helper_rotate(curAngle, 0);
      aSteps = findSteps(dAngle, "angle");
      rotate(aSteps);
      newAngle = stepsToAngle(aSteps);
      updateLocation(newAngle, 0);
    }
    delSteps = 0;
    while( delSteps < 1296)
    {
      distance = lowSensor();      // this will need to be the low sensor on the right side of the robot
      if (distance > 30)
      {
        linear(54);
        delSteps = delSteps + 54;
        deldist = stepsToDistance(54);
        updateLocation(0, deldist);
      }
      else
      {
        aSteps = findSteps(90, "angle");
        rotate(aSteps);
        distance = lowSensor();
        for(int i=0; i< 19; i++)
        {
           distance = distance + lowSensor();  // this will need to be the low sensor on the center of the robot
        }
        distance = distance/20;  
        bool obstacleCheck = checkIfObstacle(distance);   // check if the sensed object is an obstacle
        if (obstacleCheck == false)
        {  
          distanceToTravel = distance - distanceFromArmToBlock; //distance to travel
          deldist = stepsToDistance(delSteps);
          newAngle = stepsToAngle(aSteps);
          updateLocation(newAngle, deldist);
          found = true;
          break;
        }
        else
        {
          aSteps = findSteps(-90, "angle");
          rotate(aSteps);
        }
      }
    }      // this next part *shouldn't* happen
  }        // but if it does, it returns the robot to the corner of the square
    if (found == false)
    {
    deldist = stepsToDistance(-delSteps);
    steps = findSteps(deldist, "distance");
    linear(steps);
    updateLocation(0, deldist);
    }
  return 10*distanceToTravel;
}


//finds distance to travel
double findDistance(double x1, double x2, double y1, double y2){
  double distance = -1;
  double sqval = sq((x2-x1)) + sq((y2-y1));
  if(sqval > 0){ distance = sqrt(sqval); }
  distance = distance * 304.8;      //convert distance to millimeters
  return (distance);
}
/*
// function to find and store mothership data.
void findMotherShip(){
  bool found = false;
  bool orthagonal = false;
  double motherShipAngle, motherShipX, motherShipY;
  double pathX[] = {4, 2, 4, 6};
  double pathY[] = {6, 4, 2, 4};    // set points for checking, if we don't find it on the first 360
  int green = rgbSensor();                          // read the rgb sensor
  int steps = findSteps(-10, "angle");
  for (int i = 0; i <36; i++){
    // read the RGB sensor
    rotate(steps);
   // if( RGB sensor == true){
      
    }
  }
  double targetAngle = findAngle(pathX[i], pathY[i]);
  turnTo(targetAngle);
  double distance = findDistance(currentCoord[0], pathX[i], currentCoord[1], pathY[i]);
  int dSteps = findSteps(distance, "distance");
  int delSteps = 0;
}
*/
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

// funtion to rotate up to 180 degrees (90 CCW, then 90 Cw), and find the lowest distance from the lowSensor
// also takes in optional boolean for mothership,and optional double for the change in angle, these default to false & -1 respectively
void rotateScan(double distance, bool motherShip = false, double delAngle = -1){
  double lowDistAngle;
  double returnAngle = curAngle;
  int steps = findSteps(delAngle, "angle");           // this might have to change for accuracy reasons, not sure right now
  for(int angle = 0; angle < 90; angle++){      // turn 90 degrees CCW, scan after each degree
    rotate(steps);
    double dist2 = lowSensor();
    if( dist2 < distance){                      // if the new scan is lower than the lowest scan
      distance = dist2;                         // set the lowest scan to the current scan
      lowDistAngle = curAngle;                  // store the angle of the lowest scan
    }
    else if(dist2 > distance){                  // if the distance is growing, break out & go to the next function.
      break;
    }
  }
  turnTo(returnAngle);                          // go back to the original angle
  for(int angle = 0; angle < 90; angle++){      // loop to go 90 degrees CW, still scanning after each degree
    rotate(-steps);
    double dist2 = lowSensor();
    if(dist2 < distance){
      distance = dist2;
      lowDistAngle = curAngle;                  // same stuff
    }
    else if(dist2 > distance){
      break;
    }
  }
  if(motherShip = true){                        // if this function is called with the intent to scan for the mothership, maybe do a 360 scan
    turnTo(returnAngle-180);                    // though, not sure if this would be necessary/usefull
    returnAngle = curAngle;
    for(int angle = 0; angle < 90; angle++){ 
      rotate(steps);
      double dist2 = lowSensor();
      if( dist2 < distance){
        distance = dist2;
        lowDistAngle = curAngle;    
      }
      else if(dist2 > distance){                // same stuff
        break;
      }
    }
    turnTo(returnAngle);
    for(int angle = 0; angle < 90; angle++){
      rotate(-steps);
      double dist2 = lowSensor();
      if(dist2 < distance){
        distance = dist2;
        lowDistAngle = curAngle;
      }
      else if(dist2 > distance){
        break;
      }
    }
  }
  if (distance < 14){                           // rotate to the angle that had the lowest distance
    turnTo(lowDistAngle);
  }
}


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
