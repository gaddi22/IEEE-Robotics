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

  if(root = "pi"){
    //readBlockData();  //will wait until data received
    root = "ard";
  }else if(root = "ard"){
    delay(3000);
    if(testCondition){
      logVal("Traveling!", "");
      findPath(blockX[1], blockY[1]); //5,5
      findPath(blockX[0], blockY[0]); //1,6
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
  if(isNE(x, y)){ targetX-=1; targetY-=1; }
  else if(isNW(x, y)){ targetY-=1; }
  else if(isSE(x, y)){ targetX-=1; targetY-=1; }
  
  //find how far to move, and angle
  Serial.print("Destionation: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);
  double targetAngle = findAngle(targetX, targetY);
  logVal("targetAngle", targetAngle);
  double dAngle   = helper_rotate(curAngle, targetAngle); //change in angle
  double distance = findDistance(currentCoord[0], targetX, currentCoord[1], targetY);
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

//travels forward through sq. returns angle in degrees to block if found
//returns 360 if nothing found
double findBlock(){
  bool found = false;
  double distanceTravelled = 0;
  double angle = blockSensorSweep();
  if(angle < 360){
    found = true;
  }
  //travel forward scanning for block
  while(!found && distanceTravelled < 1.2){
    int dSteps = findSteps(40, "distance"); //travel 4 cm
    double trueDistance = stepsToDistance(dSteps);
    linear(dSteps);
    distanceTravelled += trueDistance;
    angle = blockSensorSweep();
    if(angle < 360){ found = true; }
  }
  updateLocation(distanceTravelled);
  return angle;
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
