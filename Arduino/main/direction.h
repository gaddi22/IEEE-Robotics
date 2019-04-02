//Handles Orientation of Robot.

#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */ 
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
double compensation_constant = 0; //initial reading for magnetometer

//arr: array of values to find std dev of
//qty: size of array/number of indexes to include in result
double stDev(double arr[], int qty) {
  double sum = 0;
  for (int index = 0; index < qty; index++) {
    sum += arr[index];
  }

  double meanSample = sum / double(qty);

  double sqDevSum = 0;

  for (int index = 0; index < qty; index++) {
    sqDevSum += pow((meanSample - arr[index]), 2);
  }

  double stDev = sqrt(sqDevSum / double(qty));
  return stDev;
}

//returns angle robot is facing
double getHeading(){
  double sampleSum = 0;
  double samples[100];
  
  for(int index = 0; index < 100; index++){
    /* Get a new sensor event */ 
    sensors_event_t event; mag.getEvent(&event);
    // Calculate the angle of the vector y,x
    float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / PI;
    // Normalize to 0-360
    if (heading < 0)
    {
      heading = 360 + heading;
    }
    samples[index] = heading;
    sampleSum+=heading;
  }

  double dev = stDev(samples, 100);
  if(dev > 1.5){ return getHeading(); }
  else{ //heading
    double heading = sampleSum/100.0;
    //Serial.print("compensation_constant: ");
    //Serial.println(compensation_constant);
    //heading -= compensation_constant;
  //  if(heading>180){ heading -= 360; }
    return heading; 
  }
}

setInitialDir(double val){
  compensation_constant = val;
}
