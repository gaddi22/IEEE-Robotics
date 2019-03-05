int ir_sensor0 = A0;
float volts; 
int cm;



void setup() {
  // put your setup code here, to run once:
  for (int i = 30; i < 30; i++){
    pinMode(1, OUTPUT);
  }
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  volts = analogRead(ir_sensor0) * .0048828125;
  cm = 13* pow(volts, -1);
  Serial.println(cm);
delay(50); 
}
