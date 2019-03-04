#include <Stepper.h>
#include <Servo.h>
#include <Arduino.h>
#include <math.h>             //used for arctan function to get angle to blocks
#include "movement.h"

String root = "ard";                //sets arduino to active
int accum = 0;                      //number of blocks picked up
double currentCoord[] = {4, 4};     //location of robot 
char rx_byte = 0;                   //byte to be read
double blockX[] = {3,5,2,7,2,7};
double blockY[] = {4,5,1,3,0,6};
void setup() {
  // put your setup code here, to run once:
  FR.setSpeed(300);
  FL.setSpeed(300);
  BR.setSpeed(300);
  BL.setSpeed(300);
  arm.attach(9);
  pincer.attach(10); 
  arm.write(0);   // initial settings for motors & servos
  // pincer.write(0) // intial servo settings are for no movement. 
  //need to look more into servo settings and getting them to cooperate with the code.
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(root == "pi"){
    receiveData();
  }
  else{
    //travel to all locations
//    for(int x = 0; x < 6; x++){
//      findPath(blockX[x], blockY[x]);
//    }
    //travels to 2,1 from 4,4
    findPath(blockX[2], blockY[2]);
    //sendData(accum++);
  }
}

//finds distance to travel
int findDistance(double x1, double x2, double y1, double y2){
  double distance = -1;
  double sqval = sq((x2-x1)) + sq((y2-y1));
  if(sqval > 0){ distance = sqrt(sqval); }
  distance = distance * 304.8;      //convert distance to millimeters
  return ((int) distance);
}

//finds path to travel to point (x,y) from currentCoord.
//currently finds straight line
void findPath(int x, int y){
  double angle = atan2( (y-currentCoord[1]), (x-currentCoord[0]) );
  angle=angle*180/3.141592653;  //radians => degrees
  Serial.print("angle");
  Serial.println(angle);
  int distance = findDistance(currentCoord[0], x, currentCoord[1], y);
  Serial.print("distance");
  Serial.println(distance);
  runPath((int) angle, distance);   //travels determined distance
  delay(7000);
}

void runPath(int angle, int distance){
  rotate(angle);    //rotate angle
  linear(distance); //travel distance in straight line
}

//gets data from raspberry pi
void receiveData(){
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    root = "ard";
    
    // check if a number was received
    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      Serial.print("Number received: ");
      Serial.println(rx_byte);
    }
    else {
      Serial.println("Not a number.");
    }
  } // end: if (Serial.available() > 0)
}

void sendData(int val){
  Serial.println(val);
  root = "pi"; //sets pi to active
}
