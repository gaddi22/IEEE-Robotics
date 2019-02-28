//d// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int echoPin = 7;
const int trigPin = 6;

long duration;
int distance; 

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW);
  delay(2);

  //send singal
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //read echo
  duration = pulseIn(echoPin, HIGH); //in microseconds
  distance = duration * 0.0343 / 2;

  if(distance < 1600){
    lcd.setCursor(0,0);
    lcd.print(distance);
    Serial.print("Distance: ");
    Serial.println(distance);
  }
}
