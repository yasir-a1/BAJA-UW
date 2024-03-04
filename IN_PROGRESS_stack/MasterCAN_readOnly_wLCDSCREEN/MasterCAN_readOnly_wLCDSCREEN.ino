#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
struct can_frame canRead;

MCP2515 mcp2515(10);

//data variable names
int temp = 0;
int tempID = 0x35;
int pot = 0;
int potID  = 0x36;


//timer settings
unsigned long currentTime;
unsigned long previousTime;
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

void screenPrint(int data1, int data2, int data3, String dataName1, String dataName2, String dataName3){
lcd.clear();
lcd.setCursor(0,0);  
lcd.print("UW BAJA");
lcd.setCursor(0,1);  
lcd.print(dataName1);
lcd.print("  ");
lcd.print(dataName2);
lcd.print("  ");
lcd.print(dataName3);
lcd.setCursor(0,2);
lcd.print(data1);
lcd.print("   ");
lcd.print(data2);
lcd.print("   ");
lcd.print(data3);
}


void setup() {
  delay(1000);
  SPI.begin();
  Serial.begin(9600);
  mcp2515.reset(); //clears prev settings
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //set settings
  mcp2515.setNormalMode();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("hi"); 
  lcd.setCursor(0,1);  
  lcd.print("UW BAJA");
  delay(1000);
  lcd.setCursor(0,1);  
  lcd.print("       ");
  lcd.print("poop");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,1);  
  lcd.print("UW BAJA");

}

void loop() {

  currentTime = millis();

  if(mcp2515.readMessage(&canRead) == MCP2515::ERROR_OK){ 
    read_data(canRead.can_id);
  }

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
    screenPrint(temp, pot, 0, "TEMP", "POT", "N/A");
    previousTime = millis();

  }

}
