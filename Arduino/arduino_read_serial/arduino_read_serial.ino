int blockX[] = {0,0,0,0,0,0};
int blockY[] = {0,0,0,0,0,0};
int blockQty = 0;
char msg = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {    // is a character available?
    msg = Serial.read();       // get the character
    Serial.println(msg);
  } // end: if (Serial.available() > 0)
}
