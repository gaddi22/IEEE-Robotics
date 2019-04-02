//Used to find distance using IR sensors from the robot to an object

//initialize IR variables
int    ir_sensor0 = A1; //center low
int    ir_sensor1 = A2; //low left side
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
 if(stDev < .9){
   rv = meanSample;
 }

 return rv;
}

int lowLeftSensor(){
  volts = analogRead(ir_sensor1) * lightSensorConversionFactor;
  cm = 13* pow(volts, -1);    //distance in cm
  
  return cm/1;
}

double highRightSensor(){
  volts = analogRead(ir_sensor1) * lightSensorConversionFactor;
  cm = 13* pow(volts, -1);    //distance in cm
  return cm;
}
