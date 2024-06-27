#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

//Create an accelaration module object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Initialize Variables
// Acceleration Related
  float acceleration_x, acceleration_y, acceleration_z;
  float net_acceleration;
  
// Velocity Related
  int dt = 0.5;       // CHANGE THIS VALUE DEPENDING ON HOW OFTEN WE LOOP (in seconds)
  float curr_x_velocity, curr_y_velocity, curr_z_velocity;
  float net_velocity, kph_net_velocity;

// Direction Related
  float thetaX, thetaY, thetaZ;
  float thetaX_degrees, thetaY_degrees, thetaZ_degrees;

/* NOTE -> These variables are initiated at the start so that we don't
           have to keep reinitializing for each cycle of the loop */

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


  // Debugging code to check raw values coming from the ADXL345 sensor
  // Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
  // Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
  // Serial.print("Z: "); Serial.println(event.acceleration.z);



  // Acceleration Calculations
  acceleration_x = event.acceleration.x;
  acceleration_y = event.acceleration.y;
  acceleration_z = event.acceleration.z;

  net_acceleration = sqrt(sq(acceleration_x) + sq(acceleration_y) + sq(acceleration_z));
  


  // Velocity Calculations
  curr_x_velocity = acceleration_x * dt;
  curr_y_velocity = acceleration_y * dt;
  curr_z_velocity = acceleration_z * dt;

  net_velocity = sqrt(sq(curr_x_velocity) + sq(curr_y_velocity) + sq(curr_z_velocity));
  kph_net_velocity = net_velocity * 3.6;



  // Direction Calculations
  thetaX = atan2(sqrt(acceleration_x * acceleration_y + acceleration_z * acceleration_z), acceleration_x);
  thetaY = atan2(sqrt(acceleration_x * acceleration_x + acceleration_z * acceleration_z), acceleration_y);
  thetaZ = atan2(sqrt(acceleration_x * acceleration_x + acceleration_y * acceleration_y), acceleration_z);
  
  thetaX_degrees = thetaX * 180.0 / PI;
  thetaY_degrees = thetaY * 180.0 / PI;
  thetaZ_degrees = thetaZ * 180.0 / PI;

  /* Maybe include if statements to make theta values into sort of like a compass */



  // Printing Results
  Serial.print("Acceleration: "); Serial.print(net_acceleration); Serial.print("m/s^2, and Speed: "); Serial.print(kph_net_velocity); Serial.print("kph");
  Serial.print(" at direction "); Serial.print(thetaX_degrees); Serial.print(thetaY_degrees); Serial.print(thetaZ_degrees);


  delay(500);
}
