#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>

struct can_frame canRead;

MCP2515 mcp2515(9);

//timer settings
unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long potReadTime = 0;
unsigned long tempReadTime = 0;
unsigned long interval = 300;

int pot;
int temp;



void read_data(int id){
  if(id == 0x37 ){
    pot = canRead.data[0];
    potReadTime = millis();
  }
  if(id == 0){
    temp = canRead.data[0];
    tempReadTime = millis();
  }
}

String time_convert(unsigned long tme){
    int hr = tme / 3600000;  // Number of milliseconds in an hour
    int mins = (tme % 3600000) / 60000;  // Get remainder from hours, convert to minutes
    int sec = (tme % 60000) / 1000;  // Get remainder from minutes, convert to seconds
    int ms = tme % 1000;  // Get leftover milliseconds                                   //Remove the number of hours and minutes, leaving only seconds.
  return ((String(hr) + ":" + String(mins) + ":" + String(sec)+ ":" + String(ms)));  //Converts to HH:MM:SS string. This can be returned to the calling function.
}


void setup() {
  delay(100);
  Serial.begin(9600);
  Serial.println("in setup");
  mcp2515.reset(); //clears prev settings
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //set settings
  mcp2515.setNormalMode();
  //pinMode(csPin, OUTPUT);
  

}

void loop() {
  currentTime = millis();

  if(mcp2515.readMessage(&canRead) == MCP2515::ERROR_OK){ 
    read_data(canRead.can_id);
  }
  if (currentTime - previousTime >= interval) {
    Serial.print("temp value is ");
    Serial.print(temp);
    Serial.print(" recorded at ");
    Serial.println(time_convert(tempReadTime));
    Serial.print("pot value is  ");
    Serial.print(pot);
    Serial.print(" recorded at ");
    Serial.println(time_convert(potReadTime));
    previousTime = millis();

  }

}
