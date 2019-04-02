//Used to find distance using IR sensors from the robot to an object

//initialize IR variables
int    ir_sensor0 = A1; //center low
int    ir_sensor1 = A2; //low left side
int    ir_sensor2 = A4; //rotating front sensor
float  volts; 
double cm;
double lightSensorConversionFactor = .0048828125;

//returns distance to an object from bottom sensor, in cm
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

int lowLeftSensor(){
  volts = analogRead(ir_sensor2) * lightSensorConversionFactor;
  cm = 13* pow(volts, -1);    //distance in cm
  
  return cm/1;
}

//rotating sensor
double highRightSensor(){
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
