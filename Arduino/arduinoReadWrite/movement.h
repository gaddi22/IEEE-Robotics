#include <Stepper.h>
#include <Servo.h>

// setup for stepper pins & declaring servos
Stepper FR(200, 32, 33, 34, 35);
Stepper FL(200, 2, 3, 4, 5);
Stepper BR(200, 22, 23, 24, 25);
Stepper BL(200, 6, 7, 8, 11);
Servo arm;
Servo pincer;

int angle = 90;
int dist = 1219;
int i;

void linear(int dist) // callable function for forwards and backwards movement
{
  if (dist >= 0){ //forwards
    int steps = dist*1.064*.5;
    for ( i = 1; i <= steps; ++i){
      FR.step(-2);
      BR.step(-2);  //looping through the motors going 2 steps at a time
      FL.step(2);
      BL.step(2);
    }
  }
  else{   // backwards
    int steps = (-dist)*1.064*.5;
    for( i = 1; i <= steps; ++i){
      FR.step(2);
      BR.step(2);
      FL.step(-2);
      BL.step(-2);
    }
  }
    return;
}

void rotate(int angle)  // callable function for rotation
{                       // number of steps may need tweaking, depends on the weight distribution; 
 //                          which will have to wait until the robot is fully built
  if (angle >= 0) // counter-clockwise rotation
  {
	for ( i = 1; i <= angle*3.44; ++i)
	{
	    FR.step(2);
	    BR.step(2);
	    FL.step(2);
	    BL.step(2);
	}
  }
  else // clockwise
  {
        for ( i = 1; i <= -angle*3.44; ++i)
        {
            FR.step(-2);
            BR.step(-2);
            FL.step(-2);
            BL.step(-2);
        }  
  }
  delay(1000);  
  return;
}

void helper_rotate(int olddeg, int newdeg){
  int deldeg = newdeg - olddeg;
  rotate(deldeg);
  return;
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
