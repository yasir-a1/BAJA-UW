#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>

struct can_frame canRead;

MCP2515 mcp2515(10);

//SPI pins 
const int csPin = 10;
const int sckPin = 0;
const int siPin = 1;
const int soPin = 2;


//data variable names
int temp = 0;
int tempID = 0x35;
int pot = 0;
int potID  = 0x36;


//timer settings
unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long potReadTime = 0;
unsigned long tempReadTime = 0;
unsigned long interval = 500;


void read_data(int id){
  if(id == potID ){
    pot = canRead.data[0];
    potReadTime = millis();
  }
  if(id == tempID){
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
  SPI.begin(sckPin, soPin, siPin, csPin);
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
  Serial.println("in loop");
  if (currentTime - previousTime >= interval) {
    Serial.print("temp is ");
    Serial.print(temp);
    Serial.print("recorded at ");
    Serial.print(time_convert(tempReadTime));
    Serial.print(" pot value is ");
    Serial.print(pot);
    Serial.print(" recorded at ");
    Serial.print(time_convert(potReadTime));
    Serial.println();
    previousTime = millis();

  }

}
