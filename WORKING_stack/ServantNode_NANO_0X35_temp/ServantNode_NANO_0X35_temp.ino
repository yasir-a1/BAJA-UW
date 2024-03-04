#include <SPI.h>
#include "DHT.h"
#include <mcp2515.h>


struct can_frame canMsg;  //creates struct for data type for canmsg
MCP2515 mcp2515(10); //pin for can modules

//pins for temp module
#define DHTPIN 5
#define DHTTYPE DHT11

unsigned long currentTime;
unsigned long previousTime;
unsigned long interval = 300; //time interval for data pushing

DHT dht(DHTPIN, DHTTYPE); //setup for temp sensor

void setup() {
  Serial.begin(9600);
  Serial.println("Just started");

  SPI.begin();
  dht.begin();

  mcp2515.reset(); //clear all preset first
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //bitrate set
  mcp2515.setNormalMode();
  
  canMsg.can_id = 0x35;   //can_id for transmission, lower id, means higher line priority
  canMsg.can_dlc = 1; //bit size of message being sent

}

void loop() {

  currentTime = millis();

  if(currentTime - previousTime >= interval){
    //

    int data = dht.readTemperature(true);
    canMsg.data[0] = data;
    mcp2515.sendMessage(&canMsg);
    Serial.print(F("Sent this message"));
    Serial.println(data);
    previousTime = millis();
  }
}
