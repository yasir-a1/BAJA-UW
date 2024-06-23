#include <Wire.h>

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out; // Outputs
int X_offset = 0, Y_offset = 0, Z_offset = 0; // Offset values

void setup() {
  Serial.begin(9600);
  Wire.begin();
  configureADXL345(); // Configure the sensor
  // Note: You should calibrate upon re-powering the sensor
  // Uncomment if you want to calibrate
  // calibrateADXL345(); // Calibrate the sensor
}

void calibrateADXL345() {
  // You should calibrate with the positive axis pointed upwards against gravity
  // You should calibrate each axis separately and change the offset constant, 
  // You need to run this code three times if you want to calibrate all three axes
  float numReadings = 500;
  // float xSum = 0;
  // float ySum = 0;
  float zSum = 0;
  Serial.print("Beginning Calibration");
  Serial.println();
  for (int i = 0; i < numReadings; i++) {
    Serial.print(i);
    Serial.println();
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true);
    X_out = (Wire.read() | Wire.read() << 8);
    Y_out = (Wire.read() | Wire.read() << 8);
    Z_out = (Wire.read() | Wire.read() << 8);
    // xSum += X_out;
    // ySum += Y_out;
    zSum += Z_out;
  }
  //// 256 is the raw value for 1g, (we calibrate at 1g hence why we take the difference)

  // X_offset = (256 - xSum / numReadings) / 4;
  // Serial.print("X_offset= " );
  // Serial.print(X_offset);
  // Y_offset = (256 - ySum / numReadings) / 4;
  // Serial.print("Y_offset= " );
  // Serial.print(Y_offset);

  Z_offset = (256 - (zSum / numReadings)) / 4;
  Serial.print("Z_offset= " );
  Serial.print(Z_offset);
  Serial.println();
  delay(1000);
  // We need to add it to the offset channel respectively =
  // Once you write to the offset channel you do not need to do it again,
  // unless you unpower and power the device
  // Each offset address is different for X, Y, and Z

  // Wire.beginTransmission(ADXL345);
  // Wire.write(0x1E);
  // Wire.write(X_offset);
  // Wire.endTransmission();
  // Wire.beginTransmission(ADXL345);
  // Wire.write(0x1F);
  // Wire.write(Y_offset);
  // Wire.endTransmission();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);
  Wire.write(Z_offset);
  Wire.endTransmission();
}

void configureADXL345() {
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // Enable measurement (D3 bit high)
  Wire.endTransmission();
  delay(10);
}

void loop() {
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  X_out = (Wire.read() | Wire.read() << 8);
  X_out = X_out / 256;
  Y_out = (Wire.read() | Wire.read() << 8);
  Y_out = Y_out / 256;
  Z_out = (Wire.read() | Wire.read() << 8);
  Z_out = Z_out / 256;

  Serial.print("Xa= ");
  Serial.print(X_out);
  Serial.print("   Ya= ");
  Serial.print(Y_out);
  Serial.print("   Za= ");
  Serial.println(Z_out);
  delay(500);
}