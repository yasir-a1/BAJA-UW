#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

int num = 200;
struct RawData{
  double x;
  double y;
  double z;
}RawVals;
int i = 0;
//Create an accelaration module object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup() {

  //Initialize the sensor make sure its there
  Serial.begin(9600);
  if (!accel.begin()) {
    Serial.println("No ADXL345 sensor detected, check wiring!");
    while (1); 
  } 
  //Set the range in which the sensor reads, can be changed do some more research 
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("ADXL345 ready!");
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);
  if(i<num){
    RawVals.x += event.acceleration.x;
    RawVals.y += event.acceleration.y;
    RawVals.z += event.acceleration.z;
    Serial.println(i);
  }
  if(i==num){
    RawVals.x /= (double)num;
    RawVals.y /= (double)num;
    RawVals.z /= (double)num;
    Serial.print("Xavg: "); Serial.print(RawVals.x); Serial.print(" ");
    Serial.print("Yavg: "); Serial.print(RawVals.y); Serial.print(" ");
    Serial.print("Zavg: "); Serial.print(RawVals.z); Serial.println(" ");
  }
  i++;
  delay(100);
}
