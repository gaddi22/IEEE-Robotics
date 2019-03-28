int blockX[] = {0,0,0,0,0,0};
int blockY[] = {0,0,0,0,0,0};
int blockQty = 0;
char msg = 0;
bool testCondition = true; //used to test a piece of code only once

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  while(testCondition){
    delay(2000);
    
    receiveBlockData();
    Serial.print("Size: ");
    Serial.println(blockQty);
    
    Serial.println("x values");
    for(int index = 0; index<blockQty; index++){
      Serial.print(blockX[index]);
      Serial.print(" ");
    }
    Serial.println("");
    
    Serial.println("y values");
    for(int index = 0; index<blockQty; index++){
      Serial.print(blockY[index]);
      Serial.print(" ");
    }

    testCondition = false;
  }
}

char receiveData(){
  char msg = -1;
  
  if (Serial.available() > 0) {    // is a character available?
    msg = Serial.read();           // get the character
  }
  
  return msg;
}

void receiveBlockData(){
  int qty = 0;
  bool x = true;
  int index = 0;
  bool runBit = true; //continue reading data

  while(runBit){
    if (Serial.available() > 0) { //check if character is available.
      char val = Serial.read();
      
      if(qty==0){ //received qty value
        qty = int(val)%48; //size value
        blockQty = qty;        
      }
      else{ 
        
      //received block location
        if(x){ //load x values
          blockX[index++] = int(val)%48;
          
          if(index >= qty){
            index = 0;
            x = false;
      }
        }else{
          blockY[index++] = int(val)%48;
          if(index >= qty){
            runBit = false;
          }
        }
      }
    }
  }
}
