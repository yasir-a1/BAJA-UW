#include <SPI.h>
#include <mcp2515.h>
#include "Adafruit_ADXL345_U.h"
#include "Adafruit_Sensor.h"
#include <Wire.h>

struct can_frame canRead;

struct acc_frame {
  struct RawData {
    double x, y, z;
  } rawVals, calibVals;
  int time;
} stitchedVals;

MCP2515 mcp2515(9);

double bias[3], scale[3];
double orientationData[6][3] = {
  {10.23, 0.07, 1.66},
  {-10.44, -0.41, 1.77},
  {-0.05, 10.18, 1.71},
  {0.16, -10.54, 1.15},
  {-0.09, -0.03, 11.72},
  {0.08, -0.05, -8.30}
}; //x,y,z then +x, -x, +y, -y, +z, -z

//timer settings
unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long potReadTime = 0;
unsigned long tempReadTime = 0;
const unsigned long interval = 300;

const int Can_Length = 8;
const int ACC_Can_ID = 0x37;

uint8_t read_ACC[8];
int temp;

void read_data(int id);
String time_convert(unsigned long time);
void interpretCanInput();
void applyCalibration();
void computeCalibration();

void setup() {
  delay(100);
  Serial.begin(9600);
  Serial.println("in setup");
  mcp2515.reset();                            //clears prev settings
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //set settings
  mcp2515.setNormalMode();
  computeCalibration();
  //pinMode(csPin, OUTPUT);
}

void loop() {
  currentTime = millis();


  if (currentTime - previousTime >= interval) {
    if (mcp2515.readMessage(&canRead) == MCP2515::ERROR_OK) {
      read_data(canRead.can_id);
    }
    /*
    Serial.println(read_ACC[0]);
    Serial.println(read_ACC[1]);
    Serial.println(read_ACC[2]);
    Serial.println(read_ACC[3]);
    Serial.println(read_ACC[4]);
    Serial.println(read_ACC[5]);
    Serial.println(read_ACC[6]);
    Serial.println(read_ACC[7]);
    */

    Serial.println(stitchedVals.rawVals.x);
    Serial.println(stitchedVals.rawVals.y);
    Serial.println(stitchedVals.rawVals.z);
    Serial.println(stitchedVals.time);
    Serial.println(stitchedVals.calibVals.x);
    Serial.println(stitchedVals.calibVals.y);
    Serial.println(stitchedVals.calibVals.z);
    Serial.println(stitchedVals.time);
    /*Serial.print("temp value is ");
    Serial.println(stitchedVals.accVals[0]);
    Serial.println(stitchedVals.accVals[1]);
    Serial.println(stitchedVals.accVals[2]);
    Serial.println(stitchedVals.time);
    Serial.print(" recorded at ");
    Serial.println(time_convert(tempReadTime));
    Serial.print("pot value is  ");
    Serial.print(temp);
    Serial.print(" recorded at ");
    Serial.println(time_convert(potReadTime));*/
    previousTime = millis();
  }
}

void read_data(int id) {
  if (id == ACC_Can_ID) {
    for (unsigned int i = 0; i < Can_Length; i++) {
      read_ACC[i] = canRead.data[i];
    }
    interpretCanInput();
    applyCalibration();
  }
  if (id == 0) {
    temp = canRead.data[0];
    tempReadTime = millis();
  }
}

String time_convert(unsigned long time) {
  int hr = time / 3600000;                                                            // Number of milliseconds in an hour
  int mins = (time % 3600000) / 60000;                                                // Get remainder from hours, convert to minutes
  int sec = (time % 60000) / 1000;                                                    // Get remainder from minutes, convert to seconds
  int ms = time % 1000;                                                               // Get leftover milliseconds                                   //Remove the number of hours and minutes, leaving only seconds.
  return ((String(hr) + ":" + String(mins) + ":" + String(sec) + ":" + String(ms)));  //Converts to HH:MM:SS string. This can be returned to the calling function.
}

void interpretCanInput() {
  int16_t xyzt[4];
  for (unsigned int i = 0; i < 4; i++) {
    xyzt[i] = read_ACC[2 * i] | (read_ACC[2 * i + 1] << 8);
  }
  stitchedVals.rawVals.x = xyzt[0] * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  stitchedVals.rawVals.y = xyzt[1] * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  stitchedVals.rawVals.z = xyzt[2] * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
  stitchedVals.time = xyzt[3];
}

void applyCalibration() {
  stitchedVals.calibVals.x = (stitchedVals.rawVals.x - bias[0])/scale[0];
  stitchedVals.calibVals.y = (stitchedVals.rawVals.y - bias[1])/scale[1];
  stitchedVals.calibVals.y = (stitchedVals.rawVals.z - bias[2])/scale[2];
}

void computeCalibration(){
  bias[0] = (orientationData[0][0] + orientationData[1][0])/2;
  bias[1] = (orientationData[2][1] + orientationData[3][1])/2;
  bias[2] = (orientationData[4][2] + orientationData[5][2])/2;

  scale[0] = (orientationData[0][0] - orientationData[1][0])/2;
  scale[1] = (orientationData[2][1] - orientationData[3][1])/2;
  scale[2] = (orientationData[4][2] - orientationData[5][2])/2;
}