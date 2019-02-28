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
  if (angle < 0) // counter-clockwise rotation
  {
    int angle_ = -angle;
  while (angle_ >= 45) // loops until the provided angle becomes less than 45; in iterations of 45 or 90
    {
    if (angle_ >= 90)  // 90 degrees
      {
        int steps = 155;
        for ( i = 1; i <= steps; ++i)
        {
          FR.step(-2);
          BR.step(-2);
          FL.step(-2);
          BL.step(-2);
        }
        angle_ = angle_-90;  
      }
    else // 45 degrees
      {
        int steps = 155*.5;
        for ( i = 1; i <= steps; ++i)
        {
          FR.step(-2);
          BR.step(-2);
          FL.step(-2);
          BL.step(-2);
        }
      angle_ = angle_-45;
      }
    } 
  }
  else // clockwise
  {
  int angle_ = angle;
  while (angle_ >= 45)
    {
    if (angle_ >= 90)
      {
        int steps = 155;
        for ( i = 1; i <= steps; ++i)
        {
          FR.step(2);
          BR.step(2);
          FL.step(2);
          BL.step(2);
          }
        angle_ = angle_-90;  
        }
    else
      {
        int steps = 155*.5;
        for ( i = 1; i <= steps; ++i)
        {
          FR.step(2);
          BR.step(2);
          FL.step(2);
          BL.step(2);
        }
    angle_ = angle_-45;
      }
    }
  }
    return;
}

void Pickup_block(){  // current attempts to get the servos to do things
    
//  pincer.write(0);
//  delay(54);
//  pincer.write(95);
  arm.write(120);
  delay(1500);

//  pincer.write(180);
//  delay(25);
//  pincer.write(95);
  arm.write(30);
  
  return;
}

void Deposit_block(){ //these don't really work right now, and need tweaking.

  arm.write(90);
  delay(1500);
//  pincer.write(0);
//  delayMicroseconds(8333);
//  pincer.write(95);

    
//  pincer.write(180);
//  delayMicroseconds(37500);
//  pincer.write(95);
  arm.write(0);

  return;  
}
