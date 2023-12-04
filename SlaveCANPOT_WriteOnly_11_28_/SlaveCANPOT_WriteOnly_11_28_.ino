#include <SPI.h>
#include <mcp2515.h>


struct can_frame canMsg;  //creates struct for data type for canmsg
MCP2515 mcp2515(10); //pin for can modules

unsigned long currentTime;
unsigned long previousTime;
unsigned long interval = 300; //time interval for data pushing

void setup() {
  Serial.begin(9600);
  Serial.println(F("Just started"));

  SPI.begin();

  mcp2515.reset(); //clear all preset first
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //bitrate set
  mcp2515.setNormalMode();
  
  canMsg.can_id = 0x36;   //can_id for transmission, lower id, means higher line priority
  canMsg.can_dlc = 1; //bit size of message being sent

  pinMode(A0, INPUT);  //pins for pot
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);//change for pot pin
}

void loop() {

  currentTime = millis();

  if(currentTime - previousTime >= interval){
    //

    int data = analogRead(A0);
    canMsg.data[0] = data;
    //data = map(data, 0, 1024, 0, 100);
    mcp2515.sendMessage(&canMsg);
    Serial.print(F("Sent this message"));
    Serial.println(data);
    previousTime = millis();
  }
}
