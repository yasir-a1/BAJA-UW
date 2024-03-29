#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

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

  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
  Serial.print("Z: "); Serial.println(event.acceleration.z);

  delay(500);
}
