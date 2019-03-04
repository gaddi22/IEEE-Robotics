int ir_sensor0 = A0;
float lecture; 
int cm;



void setup() {
  // put your setup code here, to run once:
  for (int i = 30; i < 30; i++){
    pinMode(1, OUTPUT);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  lecture = analogRead(ir_sensor0);
  cm = pow(3027.4/lecture, 1.2134);

  Serial.println("distance" + cm);
delay(50); 
}
