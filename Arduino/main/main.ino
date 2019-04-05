#include <Servo.h>
#include <Arduino.h>
#include <math.h>             //used for arctan function to get angle to blocks
#include "movement.h"
#include "direction.h"
#include "distance.h"
#include "arm.h"
#include "color.h"

//----------------State Manager Variables---------------
char   msg            = 0;          //read from pi
String root           = "pi";      //sets arduino to active
int    accum          = 0;          //number of blocks picked up
double curAngle       = 0;          //current Degees Robot is facing 
double currentCoord[] = {3.5, 3.5};     //location of robot
int    blockQty = 0;
double blockX[] = {1,5,2,7,2,7};    //blocks' Xcoordinates
double blockY[] = {6,5,1,3,0,6};    //blocks' Ycoordinates
double initialDir = 0;              //direction magnetometer says north is
double forwardDistanceFromArmToBlock = 12.0; //cm, minimum distance to pick up block
double backwardDistanceFromArmToBlock = 11.0;
bool   testCondition = true;        //used to test a single iteration
bool   nE = false;
bool   nW = false;
bool   sE = false;
double motherX;
double motherY;
double motherAngle;

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

  //IR Sensor Servo
  myservo.attach(12);
  myservo.write(90);

  //----------Color Senser----------
  tcs.begin();

  //----------Magnetometer----------
  mag.begin();
  initialDir = getHeading();
  setInitialDir(initialDir);
  
  //----------Serial Setup----------
  Serial.begin(9600);
  delay(2500);
}

void loop() {
  delay(1000);
  if(root == "pi"){
  //readBlockData();  //will wait until data received
    root = "ard";
  }else if(root == "ard"){
    delay(3000);
    if(testCondition){
      logVal("Traveling!", "");
      findPath(blockX[1], blockY[1]); //5,5   
      double blockAngle = findBlock();    // find the block, returning the angle of the servo
      double distance = blockSensor();    // take another measurement for the distance
      distance = zeroInOnBlock(distance); // look at the middle of the object
      blockAngle = myservo.read();        // reread the servo angle
      double sensorOffsetX = 176.36*sin(32.6+curAngle);  // compute the coordinates of the object
      double sensorOffsetY = 176.36*cos(32.6+curAngle);
      double blockOffsetX = distance*sin(blockAngle-90+curAngle);
      double blockOffsetY = distance*cos(blockAngle-90+curAngle);
      double objectCoordX = currentCoord[0] + sensorOffsetX + blockOffsetX;
      double objectCoordY = currentCoord[1] + sensorOffsetY + blockOffsetY;    // these are still annoying and possibly wrong
      double tarAngle = findAngle(objectCoordX, objectCoordY);            // find the target angle of the object to turn to
      turnTo(tarAngle);   
      updateLocation(0.0);
      distance = findDistance(currentCoord[0], objectCoordX, currentCoord[1], objectCoordY);
      distance = distance - 127;
      if(distance > forwardDistanceFromArmToBlock){   // if the object is further than the arm length
    //logVal("Object detected!", "");
    //Serial.println("Moving to object");
        double dtt = (distance - forwardDistanceFromArmToBlock)*10; //distance to travel
        int distanceSteps = findSteps(dtt, "distance");   // move to position
        logVal("Distancesteps: ", distanceSteps);
        linear(distanceSteps);
        updateLocation(stepsToDistance(distanceSteps));
      }
      else if(distance < backwardDistanceFromArmToBlock){     // if the object is shorter than the arm length (these are different for offest reasons)
        double dtt = (distance - backwardDistanceFromArmToBlock)*10;
        int distanceSteps = findSteps(dtt, "distance");
        linear(distanceSteps);
        updateLocation(stepsToDistance(distanceSteps));
      }
      pickup();   // pickup the block
      delay(2000);
      findPath(blockX[0], blockY[0]); //1,6
      blockAngle = findBlock();
      distance = blockSensor();
      distance = zeroInOnBlock(distance);
      blockAngle = myservo.read();
      sensorOffsetX = 176.36*sin(32.6+curAngle);  // compute the coordinates of the object
      sensorOffsetY = 176.36*cos(32.6+curAngle);
      blockOffsetX = distance*sin(blockAngle-90+curAngle);
      blockOffsetY = distance*cos(blockAngle-90+curAngle);
      objectCoordX = currentCoord[0] + sensorOffsetX + blockOffsetX;
      objectCoordY = currentCoord[1] + sensorOffsetY + blockOffsetY;    // these are still annoying and possibly wrong
      tarAngle = findAngle(objectCoordX, objectCoordY);
      turnTo(tarAngle);
      delay(2000);
      testCondition = false;
    }
  }
//  /
//    pickup();
//    deposit();
  
  /*
  delay(1000);
  
  //testing
  double distance = lowSensor();
  logVal("Distance: ", distance);
  if(distance > 20){
    //Serial.println("object too far");
    rotate(-50);
  }else if(distance > distanceFromArmToBlock){
    //logVal("Object detected!", "");
    //Serial.println("Moving to object");
    double dtt = (distance - distanceFromArmToBlock)*10; //distance to travel
    int distanceSteps = findSteps(dtt, "distance");
    logVal("Distancesteps: ", distanceSteps);
    linear(distanceSteps);
    updateLocation(0, stepsToDistance(distanceSteps));
  }else{
    Serial.println("Picking up object");
    pickup();
    deposit();
  }
  
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

//prints value to serial monitor
void logVal(String msg, double val){
  Serial.println(msg + String(val));
}

void logVal(String msg, String val){
  Serial.println(msg + val);
}


//distance: distance to object
//bool checkIfObstacle(double distance){      // these currently call lowSensor, they need to call the High right sensor
//  double a, b, c, theta;
//  a = distance;
//  b = 8.211;
//  c = sqrt(sq(a)+sq(b));
//  theta = asin(b/c);
//  int steps = findSteps(-theta, "angle");
//  rotate(steps);
//  double dist = highRightSensor();
//  if (dist <= 30){
//    for (int i = 0; i < 19; i++){         // might not need this loop at all
//      dist = dist + highRightSensor();  
//    }
//    dist = dist/20;
//    return true;
//  }
//  else{
//    return false;
//  }
//}


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

bool isNE(int x, int y){
  return (currentCoord[0] < x && currentCoord[1] < y);
}

bool isNW(int x, int y){
  return (currentCoord[0] > x && currentCoord[1] < y);
}

bool isSE(int x, int y){
  return (currentCoord[0] < x && currentCoord[1] > y);
}

//find angle from current location to coordinate. 0 is north
double findAngle(double x, double y){
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

void turnTo(double targetAngle){
  double delAngle = helper_rotate(curAngle, targetAngle);
  int steps = findSteps(delAngle, "angle");
  rotate(steps);
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
//goes to corner of sq
void findPath(int x, int y){
  //get destination
  int targetX = x; int targetY = y;
  if(isNE(x, y)){ 
    nE = true;
    targetX-=1; targetY-=1; }
  else if(isNW(x, y)){ 
    nW = true;
    targetY-=1; }
  else if(isSE(x, y)){ 
    sE = true;
    targetX-=1; targetY-=1; }
  
  //find how far to move, and angle
  Serial.print("Destination: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);
  double targetAngle = findAngle(targetX, targetY);
  logVal("targetAngle", targetAngle);
  double dAngle   = helper_rotate(curAngle, targetAngle); //change in angle
  double distance = findDistance(currentCoord[0], targetX, currentCoord[1], targetY);
  distance = distance - 127;                  // don't enter the square.
  logVal("distance", distance/304.8);
  int    dSteps   = findSteps(distance, "distance");
  int    aSteps   = findSteps(dAngle,   "angle"   );
  
  double trueDistance = stepsToDistance(dSteps);
  double trueAngle    = stepsToAngle(aSteps);
  
  //move
  while(abs(getHeading() - targetAngle) > .5){
    Serial.println("Turning..."); 
    turnTo(targetAngle);
    updateLocation(0);
    logVal("Heading: ", getHeading());
  }
  linear(dSteps);
  updateLocation(trueDistance);
  delay(3000);
}

// funtion to rotate up to 180 degrees (90 CCW, then 90 Cw), and find the lowest distance from the lowSensor
// also takes in optional double for the change in angle, this defaults to false & -1 respectively
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
  bool found = false;
//  double motherShipAngle, motherShipX, motherShipY;
  double pathX[] = {2, 6};
  double pathY[] = {4, 4};    // set points for checking, if we don't find it on the first 360
  bool isgreen = false;                          
  int steps = findSteps(15.0, "angle");
  for (int i = 0; i <24; i++){            // do 360 degree check on the starting point.
    isgreen = isGreenPresent();
    if( isgreen == true){                 // if green is present in the current direction, break out of the loop
      found = true;
      break;
    }
    rotate(steps);                        // rotate 10 degrees
  }
  if (found != true){                     // if the mothership isn't found, 
    turnTo(0.0);
    delay(250);
    for (int i = 0; i <= 1; i++){         //  go to the next point and do a scan there
      double targetAngle = findAngle(pathX[i], pathY[i]);
      turnTo(targetAngle);
      double distance = findDistance(currentCoord[0], pathX[i], currentCoord[1], pathY[i]);
      int dSteps = findSteps(distance, "distance");
      linear(dSteps);
      turnTo(0.0);
      if (i < 1){ turnTo(150.0); }
      else { turnTo(-30.0); }
      for (int i = 0; i < 16; i++){       // scan from curAngle+120 to curAngle-120
                                          
        isgreen = isGreenPresent();
        if( isgreen == true){
          found = true;                   // if we find it, set found to true, and break out of the loop
          break;
        }
        rotate(steps);                    // first one is CCW
      }
      if( found == true){                 // if we found the mothership, break out of the loop
        break;                            // this may seem redundant, but we have to do this twice since there are 2 for loops
      }
      turnTo(0.0);
    }
  }
  else{
    double dtt= lowSensor();              // dtt = distance to target
                                          // need to step towards the source of the green light, in increments of our sensor range; 14cm, or 140 mm (doing 130 mm for now)
    double distance = 130.0;
    int dSteps = findSteps(distance, "distance");
    int iterations = 0;
    while (dtt > 14 && iterations < 6){
      linear(dSteps);
      delay(150);
      dtt = lowSensor();                   // sensor scan of the lowSensor, and the highSensor   high sensor isn't currently set up, but will be soon(TM)
      iterations += 1;
    }
  }
}


void alignMotherShip(){
    // need to find out if we are pointed at a corner
    // for aligning the with the mothership
    double dtt = lowSensor();          // distance to target
    rotateScan(dtt);
    double returnAngle = curAngle;
    int steps = findSteps(30.0, "angle");  // this value might need to be higher
    rotate(steps);
    bool isgreen2 = isGreenPresent();
    turnTo(returnAngle);
    rotate(-steps);
    bool isgreen1 = isGreenPresent();
    turnTo(returnAngle);
    if(isgreen1 == true && isgreen2 == true){
      return;
    }
    else {
      double dist = lowSensor();                       // dist from sensor to corner
      double x = 142.5;                                // dist from sensor to edge of wheel
      double theta = asin(x/(sqrt(sq(x)+sq(dist))));   // angle 
      steps = findSteps(theta, "angle");               // steps for rotation
      int dSteps = findSteps(108.24, "distance");          // steps for 1/2 the distance of the mothership
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
      dtt = lowSensor();
      motherAngle = curAngle;
      motherX = currentCoord[0] + ((.488845)*sin(curAngle)) + ((.30208)*sin(curAngle+90));
      motherY = currentCoord[1] + ((.488845)*sin(curAngle)) + ((.30208)*sin(curAngle+90));
      Serial.print("Mothership X coordinate: ");
      Serial.println(motherX);
      Serial.print("Mothership Y coordinate: ");
      Serial.println(motherY);
      Serial.print("Mothership angle: ");
      Serial.println(motherAngle);
      return;
    }
}


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

//moves robot to new angle and moves distance
void runPath(int aSteps, int dSteps){
  rotate(aSteps); //rotate delta angle 
  linear(dSteps); //travel distance in straight line
}

//trueAngle:    angle robot rotated
//trueDistance: distance robot moved
void updateLocation(double trueDistance){
  //curAngle         = curAngle + trueAngle;
  double newCurAngle = getHeading();
  double trueAngle   = newCurAngle - curAngle;
  double rad         = degToRad(curAngle);           //angle in radians
  curAngle           = newCurAngle;
  //new location, convert distance to block location
  if(trueDistance > 0){
    currentCoord[0] += sin(rad) * trueDistance/304.8;
    currentCoord[1] += cos(rad) * trueDistance/304.8;
    String display = String(currentCoord[0]) + ", " + String(currentCoord[1]); 
    logVal("New location: ", display);
    logVal("New angle: ", curAngle);
  }
}

/*
//travels forward through sq. returns angle in degrees to block if found
//returns 360 if nothing found
double findBlock(){
  bool found = false;
  double distanceTravelled = 0;
  double angle = blockSensorSweep();
  if(angle < 135){
    found = true;
  }
  //travel forward scanning for block
  while(!found && distanceTravelled < 1.2){
    int dSteps = findSteps(40, "distance"); //travel 4 cm
    double trueDistance = stepsToDistance(dSteps);
    linear(dSteps);
    distanceTravelled += trueDistance;
    angle = blockSensorSweep();
    if(angle < 135){ found = true; }
  }
  updateLocation(distanceTravelled);
  return angle;
}
*/
//blockNum: block we are currently searching for
//loc:      location of block currently being searched for
bool findBlock(){
  //values returned
  int distanceToBlock = 0;
  int angleToBlock    = 0;

  //Values to solve problem
  bool found = false;
  double angleToTurn = -90;        //left of right
  double angleToTurn2 = -45;
  if(nE){
    angleToTurn = 90;
    angleToTurn2 = 135;
  }
  else if(nW){
    angleToTurn = 0;
    angleToTurn2 = 45;
  }
  else if(sE){
    angleToTurn = 180;
    angleToTurn2 = -135;
  }
  double distanceToTravel = 304.8;      //1 ft in mm
  double dSteps = findSteps(175, "distance");
  double distance = 0;  //distance travelled

  //---------------1st Pass---------------
  turnTo(angleToTurn); //turns until it reaches angleToTurn
  updateLocation(0);
  double lowD = lowSensor(); 
  found = lowD < 20;
  double newAngle = 0;
  while ( newAngle > -180 && !found){
    //Serial.println("1");
    int steps = findSteps(1, "angle");
    rotate(-steps);
    double angleRotated = stepsToAngle(steps);
    newAngle -= angleRotated;
    lowD = lowSensor(); 
    found = lowD < 20;
  }
  updateLocation(0);
  

  if(!found){//---------------2nd Pass---------------
    turnTo((newAngle - angleToTurn)/2);
    updateLocation(0);
    linear(dSteps);
    distance += 150;
    turnTo(angleToTurn2);
    updateLocation(stepsToDistance(dSteps));
    lowD = lowSensor();
    found = lowD < 20;
    newAngle = 0;
    while(newAngle < 270 && !found){
      Serial.println("2");
      int steps = findSteps(1, "angle");
      rotate(-steps);
      double angleRotated = stepsToAngle(steps);
      newAngle -= angleRotated;
      lowD = lowSensor(); 
      found = lowD < 20;
    }
    updateLocation(0);
  }
  
  /*
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
  */
  if(found){
   Serial.println("Found"); 
  }else{
    Serial.println("Not Found"); 
    return 0; //false, not found
  }
  return 1;
}

//realigns robot to 0 degrees. Fixing errors in rotation accumulated over time
void realign(){
  turnTo(0);
  updateLocation(0);
  //find difference from 0 degrees. Rotate until 0 degrees reached
  while(abs(curAngle) > 0.35){ 
    double aSteps = findSteps(-curAngle, "angle");
    rotate(aSteps);
    updateLocation(0);
  }
}

void returnToCenter(){
  findPath(4,4);
  if(equal(currentCoord[0], 3)){
    if(equal(currentCoord[1], 3)){
      turnTo(45);
    }else{ turnTo(135); }
  }else if(equal(currentCoord[0], 4)){
    if(equal(currentCoord[1], 3)){
      turnTo(-45);
    }else{ turnTo(-135); }
  }
  int dSteps = findSteps(17.96, "distance");     //travel to center of square
  double trueDistance = stepsToDistance(dSteps);
  linear(dSteps);
  updateLocation(trueDistance);
}
/*
// function to zero in on a block, to be run either in or after findBlock function
// this attempts to aim at the center of a block, & return the shortest distance from the robot to the block.
// hopefully to deal with angled blocks
double zeroInOnBlock(double distance){
  double CCWAngle, CWAngle;
  double returnAngle = myservo.read();
  for(int angle = 1; angle <= 10; angle++){      // turn 10 degrees CCW, scan after each degree
    myservo.write(returnAngle+angle);
    double dist2 = blockSensor();
    if( dist2 < distance){                      // if the new reading is lower than the lowest reading
      distance = dist2;                         // set the lowest reading to the current reading
    }
    else if(dist2 > 17){                  // if the distance is growing, break out & go to the next function.
      break;
    }
    CCWAngle = curAngle;
  }
  myservo.write(returnAngle);                          // go back to the original angle
  for(int angle = 1; angle <= 10; angle++){      // loop to go 10 degrees CW, still scanning after each degree
    myservo.write(returnAngle+angle);
    double dist2 = blockSensor();
    if( dist2 < distance){                      // if the new reading is lower than the lowest reading
      distance = dist2;                         // set the lowest reading to the current reading
    }
    else if(dist2 > 17){                  // if the distance is growing, break out & go to the next function.
      break;
    }
    CWAngle = curAngle;
  }
  double tarAngle = (CCWAngle + CWAngle)/2;
  myservo.write(tarAngle);
  return distance;
}
*/

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
