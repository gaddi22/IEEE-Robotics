/*
 * Adafruit TCS34725 RGB Sensor Demo Code
 * Written by David Archuleta and Nevid Harji...ish
  */
 
#include <Wire.h>                                                 //include the I2C library to communicate with the sensor
#include "Adafruit_TCS34725.h"                                    //include the sensor library
  
byte gammatable[256];                                             // our RGB -> eye-recognized gamma color
 
                                                                  //Create an instance of the TCS34725 Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
 
void setup() {
  Serial.begin(9600);                                             //Sart serial comms @ 9600 (you can change this)
  Serial.println("Color View Test");                              //Title info            
 
  if (tcs.begin()) {                                              //if the sensor starts correctly
    Serial.println("Found sensor");                               //print the happy message
  } else {                                                        //if the sensor starts incorrectly
    Serial.println("No TCS34725 found ... check your connections");//print the not so happy message
    while (1); // halt!
  }
 
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
}
} 

void loop(){

  String color = colorSense();
  Serial.println(color);
}
 
String colorSense() {
  uint16_t clear, red, green, blue;                                 //declare variables for the colors
 
  tcs.setInterrupt(false);                                          // turn on LED
 
  delay(60);                                                        // takes 50ms to read
 
  tcs.getRawData(&red, &green, &blue, &clear);                      //read the sensor
 
  tcs.setInterrupt(true);                                           // turn off LED
 
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  String color = "color";
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();
  return color;
  //delay(1000);                                                  // delay can be added depending on how many readings we need
 

}
