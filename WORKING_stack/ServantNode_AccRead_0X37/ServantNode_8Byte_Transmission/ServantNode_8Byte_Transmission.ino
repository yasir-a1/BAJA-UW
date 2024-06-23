// Combined Acceleration reading and canBus communication for servant node
// Run on Arduino NANO
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <mcp2515.h>

//Variables for the running of the code
const unsigned int interval = 300;
const int Can_CS_Pin = 10;
const int Can_Length = 8;
const int Can_ID = 0x37;

//Function Declarations
bool CanInitialize();  // returns 1 for fault, 0 for no fault
bool AccInitialize();  // returns 1 for fault, 0 for no fault

//
struct can_frame canPacket;  // creates struct for data type for AccPacket
volatile unsigned long currentTime;
volatile unsigned long previousTime;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);  // Create an accelaration module object
MCP2515 mcp2515(Can_CS_Pin);                                       // pin for can modules
int iAccValues[3] = { 0xf0f0, 0x0f0f, 0xAAAA};                                   // Stores values for

void setup() {

  Serial.begin(9600);
  Serial.println("Initialize CanBus and Accelerometer");
  if (CanInitialize()) {
    Serial.println("Failed to initialize CanBus, Check wiring");
    while (1) {}
  }
  if (AccInitialize()) {
    Serial.println("Failed to initialize Accelerometer, Check wiring");
    while (1) {}
  }
  Serial.println("Finished Initialization");
}

void loop() {

  if (millis() - previousTime >= interval) {
    /*iAccValues[0] = accel.getX();
    iAccValues[1] = accel.getY();
    iAccValues[2] = accel.getZ();*/
    unsigned long tInterval = previousTime - millis();
    previousTime = millis();
    for (int i = 0; i < 3; i++) {
      Serial.println(iAccValues[i]);
      canPacket.data[2 * i] = iAccValues[i] & 0xFF;
      canPacket.data[2 * i + 1] = (iAccValues[i] >> 8) & 0XFF;
    }
    canPacket.data[6] = tInterval & 0xFF;
    canPacket.data[7] = (tInterval >> 8) & 0xFF;
    mcp2515.sendMessage(&canPacket);
    //Serial.println("Sent Values X: %d, Y: %d, Z: %d, Time: %d", iAccValues[0], iAccValues[1], iAccValues[3], tInterval);
  }
}

bool CanInitialize() {
  // Initializes Can communication, returns true for a fault
  SPI.begin();

  mcp2515.reset();                            // clear all preset first
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // bitrate set
  mcp2515.setNormalMode();

  canPacket.can_id = Can_ID;       // MUST MATCH ID for MASTER NODE, can_id for transmission, lower id, means higher line priority
  canPacket.can_dlc = Can_Length;  // Length of message being sent [Bytes]

  Serial.println("mcp2515 Ready");
  return 0;
}

bool AccInitialize() {
  //Initializes Accelerometer, returns true on fault
  if (!accel.begin()) {
    Serial.println("No ADXL345 sensor detected, check wiring!");
    return 1;
  }
  // Set the range in which the sensor reads, can be changed do some more research
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("ADXL345 ready!");
  return 0;
}