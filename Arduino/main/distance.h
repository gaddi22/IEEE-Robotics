//Used to find distance using IR sensors from the robot to an object
Servo sensor;
#define sensor_Min 0
#define sensor_Max 180
#define initialSensor 90

//initialize IR variables
Servo myservo; 
int    ir_sensor0 = A2; //center low
//int    ir_sensor1 = A1; //low left side
int    ir_sensor2 = A1; //rotating front sensor
int    pos        =  0; //variable to hold sensor/servo angle
float  volts; 
double cm;
double lightSensorConversionFactor = .0048828125;

//returns distance to an object from bottom, center, sensor, in cm
double lowSensor(){
  double sampleSum = 0;
  int samples[100];

  for(int reading = 0; reading < 100; reading++){
    volts = analogRead(ir_sensor0) * lightSensorConversionFactor;
    cm = 13* pow(volts, -1);    //distance in cm
    samples[reading] = cm;
    sampleSum += cm;
  }

  double meanSample = sampleSum / 100.0;

  double sqDevSum = 0.0;

  for(int sample = 0; sample < 100; sample++){
    sqDevSum += pow((meanSample - double(samples[sample])), 2);
  }

 double stDev = sqrt(sqDevSum/100.0);

 double rv = 100;
 if(stDev < .95){
   rv = meanSample;
 }

 return rv;
}

// scans the area in front of the rover, and returns the angle of the first object it sees
double sensorScan(){
  sensor.write(sensor_Min);
  double distance = lowSensor();
  int currentA = sensor.read();
  while(currentA < sensor_Max){
    sensor.write(currentA + 2);
    distance = lowSensor();
    currentA = sensor.read();
    if(distance < 14){
      break;
    }
  }
  return currentA;
}

int lowLeftSensor(){
  volts = analogRead(ir_sensor2) * lightSensorConversionFactor;
  cm = 13* pow(volts, -1);    //distance in cm
  
  return cm/1;
}

//rotating sensor
//returns distance to object
double blockSensor(){
  double sampleSum = 0;
  double samples[100];
  for(int index = 0; index<100; index++){
    volts = analogRead(ir_sensor2) * lightSensorConversionFactor;
    cm = 13* pow(volts, -1);    //distance in cm
    samples[index] = cm;
    sampleSum += cm;
  }

  //std Dev of sample
  double dev = stDev(samples, 100);

  double rv = 100.0;
  if(dev < .95){
    rv = sampleSum / 100.0;
  }
  return rv;
}

//returns angle at which object was found
double blockSensorSweep(){
  myservo.write(90);
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    double distance = blockSensor();
    if(distance <= 16.0){ 
      distance = blockSensor();
      if(distance <= 16.0){ 
        pos += 1;
        myservo.write(pos);
        delay(15);
        distance = blockSensor();
          if (distance < 16.0){ return pos; }   // doublecheck to make sure that something is there.
      }
    }
  }
  pos = 90;
  myservo.write(pos);  //return to center
  return 360; //nothing found
}
