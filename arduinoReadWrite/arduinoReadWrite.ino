String root = "ard"; //sets arduino to active
int accum = 0;
char rx_byte = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(root == "pi"){
    receiveData();
  }
  else{
    sendData(accum++);
  }
}

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
