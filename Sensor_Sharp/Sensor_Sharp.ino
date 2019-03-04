int ir_sensor0 = A0;
float lecture; 
int cm;



void setup() {
  // put your setup code here, to run once:
  for (int i = 30; i < 38; i++){
    pinMode(1, OUTPUT);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  lecture = analogRead(ir_sensor0);
  cm = pow(3027.4/lecture, 1.2134);

  if(cm >= 0 && cm <10){
    for(int i = 30; i<31; i++) {
      digitalWrite(i,HIGH);
    }
    for(int i = 31; i <38; i++) {
      digitalWrite(i,LOW);
    }
}
  else if (cm>=10 && cm<20){
    for(int i=30; i<32;i++){
      digitalWrite (i,HIGH);
    }
    for(int i = 32; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

  else if (cm>=20 && cm<30){
    for(int i=30; i<33;i++){
       digitalWrite(i,HIGH);
    }
    for(int i=33; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

else if (cm>=30 && cm<40){
    for(int i=30; i<34;i++){
       digitalWrite(i,HIGH);
    }
    for(int i=34; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

  else if (cm>=40 && cm<50){
    for(int i=30; i<35;i++){
       digitalWrite(i,HIGH);
    }
    for(int i=35; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

else if (cm>=50 && cm<60){
    for(int i=30; i<36;i++){
       digitalWrite(i,HIGH);
    }
    for(int i=36; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

else if (cm>=60 && cm<70){
    for(int i=30; i<37;i++){
       digitalWrite(i,HIGH);
    }
    for(int i=37; i<38;i++){
      digitalWrite(i,LOW);
    }
  }

else if (cm>=70 && cm<80){
    for(int i=30; i<38;i++){
       digitalWrite(i,HIGH);
    }
    
  }

else{
  for(int i=30; i<38;i++){
    digitalWrite(i,LOW);
  }
}

delay(50); 

}
