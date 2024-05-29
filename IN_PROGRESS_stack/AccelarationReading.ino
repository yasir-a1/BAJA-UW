#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

//Create an accelaration module object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Variables for Speed calculation 
float in_veloc_x = 0.0; 
float in_veloc_y = 0.0; 
float in_veloc_z = 0.0; 
float dt = 0.1; 

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

  // Reading the data values
  sensors_event_t event;
  accel.getEvent(&event);

  // Sensitivity scale factor of 16g
  

  // Converting data values into g force data
  float accel_x_raw = event.acceleration.x;
  float accel_y_raw = event.acceleration.y;
  float accel_z_raw = event.acceleration.z;

  // Converting g force data into acceleration (m/s^2) (define var beginning)
  float accel_x = accel_x_raw;
  float accel_y = accel_y_raw;
  float accel_z = accel_z_raw;

  // float accel_x = map(accel_x_raw, , , );
  // float accel_y = map(accel_y_raw, , , );
  // float accel_z = map(accel_z_raw, , , );

  

  // Net acceleration
  float accel_net = sqrt(sq(accel_x)+sq(accel_y)+sq(accel_z));

  // Converting acceleration to speed using integrals
  float veloc_x = accel_x * dt;
  float veloc_y = accel_y * dt;
  float veloc_z = accel_z * dt;
  
  // Setting initial velocities to the new velocity for next calculation
  in_veloc_x = veloc_x;
  in_veloc_y = veloc_y;
  in_veloc_z = veloc_z;

  // Converting velocity to speed by taking its magnitude
  float speed_ms = sqrt(sq(veloc_x)+sq(veloc_y)+sq(veloc_z));
  float speed_kmh = speed_ms * 3.6; // Converted m/s to km/h


  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
  Serial.print("Z: "); Serial.println(event.acceleration.z);
  Serial.print("Speed: "); Serial.print(speed_ms); Serial.print(" ");
  Serial.print("Acceleration: "); Serial.print(accel_net); Serial.print(" ");


  delay(1000);
}
