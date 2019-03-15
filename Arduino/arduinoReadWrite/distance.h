int ir_sensor0 = A0;
float volts; 
double cm;

double lightSensorConversionFactor = .0048828125;

//returns distance to an object from bottom sensor
double lowSensor(){
  volts = analogRead(ir_sensor0) * lightSensorConversionFactor;
  cm = 13* pow(volts, -1);		//distance in cm
  return cm;
}
