
#include <Servo.h>

Servo myservo;  // servo object for the arm
Servo myservo1; // servo objejct for the pincer

//Defining min and max values for arm/pincer servos
#define ARM_MIN 0
#define ARM_MAX 120
#define PIN_MIN 0
#define PIN_MAX 60

int angleArm = 97;    // angle for arm servo to pickup block
int anglePin = 55;  // angle for the pincer servo to grab the block 
int inAngle = 0;


void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object arm
  myservo1.attach(10);  // attaches the servo on pin 10 to the servo object pincer
  myservo.write(inAngle);
  myservo1.write(inAngle);
}


void loop() {
pickup();  //Picks up block
delay(1000);
deposit(); //Drops block

}

//function to pick up block
void pickup() {
 myservo1.write(constrain(anglePin, PIN_MIN, PIN_MAX));  // opens the pincer the correct amount 
 delay(2000);
 
 myservo.write(constrain(angleArm, ARM_MIN, ARM_MAX)); //drop arm to get block (drop it like its hot) 
 Serial.println(angleArm); 
 delay(2000);

 myservo1.write(constrain(anglePin-27.5, PIN_MIN, PIN_MAX)); //closes the pincer around the block
 delay(2000);

 myservo.write(-angleArm); //raises the arm to its original position
 Serial.println(angleArm); //
 delay(2000); 
}
void deposit() {
  myservo.write(constrain(angleArm-20,ARM_MIN, ARM_MAX)); //drops the arm to deposit block into mothership
  delay(1000);
  myservo1.write(constrain(anglePin, PIN_MIN, PIN_MAX));  // opens the pincer the correct amount 
  delay(1 000);
  myservo1.write(constrain(inAngle, PIN_MIN, PIN_MAX));  //closes the pincer to return to initial position
  delay(1000);
  myservo.write(constrain(inAngle, ARM_MIN, ARM_MAX));  //raises the arm to return to initial position
  delay(3000);
  
}
