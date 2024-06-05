// Combined Acceleration reading and canBus communication for servant node
// Run on Arduino NANO
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <mcp2515.h>

bool CanInitialize(); // returns 1 for fault, 0 for no fault
bool AccInitialize(); // returns 1 for fault, 0 for no fault

volatile unsigned long currentTime;
volatile unsigned long previousTime;
const int interval = 300;                                     // time interval for data pushing (ms)
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345); // Create an accelaration module object
struct can_frame canPacket;                                       // creates struct for data type for AccPacket
MCP2515 mcp2515(10);                                              // pin for can modules
int iAccValues[3] = {0, 0, 0};                                    // Stores values for 

void setup()
{
    Serial.begin(9600);
   
    Serial.println("Initialize CanBus and Accelerometer");
    if (CanInitialize())
    {
        Serial.println("Failed to initialize CanBus");
        while (1)
        {
            
        }
    }
    if (AccInitialize())
    {
        Serial.println("Failed to initialize Accelerometer");
        while (1)
        {
            
        }
    }
    Serial.println("Finished Initialization");
}

void loop()
{
    currentTime = millis();

    if (currentTime - previousTime >= interval)
    {
        iAccValues[0] = accel.getX();
        iAccValues[1] = accel.getY();
        iAccValues[2] = accel.getZ();
        for (int i = 0; i < 3; i++)
        {
            canPacket.data[2 * i] = iAccValues[i] & 0xFF;
            canPacket.data[2 * i + 1] = (iAccValues[i] >> 8) & 0XFF;
        }
        int tInterval = previousTime - millis();
        canPacket.data[6] = tInterval & 0xFF;
        canPacket.data[7] = (tInterval >> 8) & 0xFF;
        mcp2515.sendMessage(&canPacket);
        previousTime = millis();
        //Serial.println("Sent Values X: %d, Y: %d, Z: %d, Time: %d", iAccValues[0], iAccValues[1], iAccValues[3], tInterval);
    }
}

bool CanInitialize()
{
    // Initializes Can communication, returns true for a fault
    SPI.begin();

    mcp2515.reset();                           // clear all preset first
    mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); // bitrate set
    mcp2515.setNormalMode();

    canPacket.can_id = 0x37; // MUST MATCH ID for MASTER NODE, can_id for transmission, lower id, means higher line priority
    canPacket.can_dlc = 8;   // Length of message being sent [Bytes]

    pinMode(A0, INPUT); // pins for sensor used
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH); // change for pot pin

    Serial.println("mcp2515 Ready");
    return 0;
}

bool AccInitialize()
{
    //Initializes Accelerometer, returns true on fault
    if (!accel.begin())
    {
        Serial.println("No ADXL345 sensor detected, check wiring!");
        return 1;
    }
    // Set the range in which the sensor reads, can be changed do some more research
    accel.setRange(ADXL345_RANGE_16_G);
    Serial.println("ADXL345 ready!");
    return 0;
}