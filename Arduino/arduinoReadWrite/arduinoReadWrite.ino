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
int    blockX[] = {3,5,2,7,2,7};    //blocks' Xcoordinates
int    blockY[] = {4,5,1,3,0,6};    //blocks' Ycoordinates
int    blockQty = 0;
double    distanceFromArmToBlock = 12; //cm, minimum distance to pick up block

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
  delay(2500);
}

void loop() {
  delay(25);
  
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
        qty = int(val)%48; //size value
        blockQty = qty;        
      }
      else{ 
        
      //received block location
        if(x){ //load x values
          blockX[index++] = int(val)%48;
          
          if(index >= qty){
            index = 0;
            x = false;
      }
        }else{
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
