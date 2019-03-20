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
Servo arm;
Servo pincer;

int angle;
int dist;
int i;
double truangle;

double angleConversionFactor    = 2.025*8;
double distanceConversionFactor = 1.064 *4;

void linear(int steps) // callable function for forwards and backwards movement
{

    for ( i = 0; i < steps; ++i){    //speed at 2.083 rev/s tweaking the 2nd delay can tweak the speed
      digitalWrite(Pulse_FL, HIGH);  //currently using 4 microsteps, this is being taken into account in
      digitalWrite(Pulse_FR, HIGH);  // findSteps function
      digitalWrite(Pulse_BL, HIGH);
      digitalWrite(Pulse_BR, HIGH);
      delayMicroseconds(200);
      digitalWrite(Pulse_FL, LOW);
      digitalWrite(Pulse_FR, LOW);
      digitalWrite(Pulse_BL, LOW);
      digitalWrite(Pulse_BR, LOW);
      delayMicroseconds(400);
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

void Pickup_block(){  // current attempts to get the servos to do things
    int current_angle = pincer.read();
    pincer.write(current_angle-65);
    current_angle = current_angle-65;
    arm.write(120);
    delay(1500);

    pincer.write(current_angle+30);
    current_angle = current_angle+30;
    arm.write(30);
  return;
}

void Deposit_block(){ //these don't really work right now, and need tweaking.
  int current_angle = pincer.read();
  arm.write(90);
  delay(500);
  pincer.write(current_angle-10);
  current_angle = current_angle-10;
  delay(1000);
    
  pincer.write(current_angle+45);
  arm.write(0);

  return;  
}

double stepsToDistance(int steps){
  return steps/distanceConversionFactor;
}

double stepsToAngle(int steps){
  return steps/angleConversionFactor;
}
