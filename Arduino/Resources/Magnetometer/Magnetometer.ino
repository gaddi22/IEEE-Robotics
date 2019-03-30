#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_LSM303_U.h>
/* Assign a unique ID to this sensor at the same time */ Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
void setup(void)
{
Serial.begin(9600);
Serial.println("Magnetometer Test"); Serial.println("");
  /* Initialise the sensor */
  if(!mag.begin())
  {
/* There was a problem detecting the LSM303 ... check your connections */ Serial.println("Ooops, no LSM303 detected ... Check your wiring!"); while(1);
} }
void loop(void)
{
  /* Get a new sensor event */ 
  sensors_event_t event; mag.getEvent(&event);
  double sampleSum = 0;
  
  double samples[1000];
  for(int index = 0; index < 1000; index++){
    
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
  Serial.print("Compass Heading: "); 
  Serial.println(sampleSum/1000.0); 
}
