#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include "SdFat.h"

struct can_frame canRead;

//Timer variables
unsigned long currentTime;
unsigned long previousTime = 0;
unsigned long accelerationReadTime = 0;
unsigned long interval = 500;

//SD card variables
#define FILE_BASE_NAME "Data"
char fileName[13] = FILE_BASE_NAME "00.csv";
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
bool closeFile = false;

//Sensor data variables
int acceleration = 0;

//Object declaration
MCP2515 mcp2515(10);
SdFat sd;
SdFile file;

//Function used to read data from a slave node
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

//Function used to convert the timestamp to milliseconds
String time_convert(unsigned long tme){
    int hr = tme / 3600000;  // Number of milliseconds in an hour
    int mins = (tme % 3600000) / 60000;  // Get remainder from hours, convert to minutes
    int sec = (tme % 60000) / 1000;  // Get remainder from minutes, convert to seconds
    int ms = tme % 1000;  // Get leftover milliseconds                                   //Remove the number of hours and minutes, leaving only seconds.
  return ((String(hr) + ":" + String(mins) + ":" + String(sec)+ ":" + String(ms)));  //Converts to HH:MM:SS string. This can be returned to the calling function.
}

//Function used to write the headers of the CSV file
void writeHeader(String column1) {
    file.print("Time of collection for " + column1);
    file.print(F(","));
    file.print(column1);
    file.println();
}

//Function used to input data into the CSV file
//call the function and pass the actual sensor data 
//and the time it was recorded
void logData(int data1, int timestamp1) {
    file.print(data1);
    file.print(F(","));
    file.print(timestamp1);
    file.println();
}

//Function used to close the file
void fileClose(){
    closedFile = true;
}

//Setup method
void setup() {

    delay(100);
    Serial.begin(9600);
    Serial.println("in setup");

    //Initializes the MCP2515
    mcp2515.reset(); //clears prev settings
    mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //set settings
    mcp2515.setNormalMode();

    
    //pinMode(csPin, OUTPUT);

    //Set the interrupt pin as input with pull up resistor
    pinMode(2, INPUT_PULLUP);
    
    //Attach the interrupt
    attachInterrupt(digitalPinToInterrupt(2), fileClose, CHANGE);

    while(!Serial) {
        ; //Waits for serial port to connect, only needed for native USB port
    }

    //Initializes SD card, ensures everything is working
    if(!sd.begin(10, SD_SCK_MHZ(50))){
        Serial.println("Initialization failed. Things to check:");
        while(1);
    }
    else{
        Serial.println("Wiring is correct and a SD card is present.");
    }

    //Checks to see if the file name exists, if it does,
    //then it will make modifications to the fileName 
    //variable and change it by incrementing the file name
    while(sd.exists(filename)){
        if(fileName[BASE_NAME_SIZE + 1] != '9'){
            fileName[BASE_NAME_SIZE + 1]++;
        }
        else if(fileName[BASE_NAME_SIZE] != '9'){
            fileName[BASE_NAME_SIZE + 1] = '0';
            fileName[BASE_NAME_SIZE]++;
        }
    }

    //Open/creates a file with the fileName 
    //variable as the name of it
    if(!file.open(fileName, O_CREAT | O_WRITE | O_APPEND)){
        Serial.print("Error opening");
        Seria;.println(fileName);
        return;
    }

    //Prints to serial monitor what the name of the file
    //the module is logging to
    Serial.print(F("Logging to:"));
    Serial.println(fileName);
    writeHeader("Acceleration");
    Serial.println(F("Press button to stop code"));
}

//Main loop
void loop() {
    //Stores the current time
    currentTime = millis();

    //Sees if there is a incoming CAN message to read from a sensor
    if(mcp2515.readMessage(&canRead) == MCP2515::ERROR_OK){ 
        read_data(canRead.can_id);
    }

    //Stores the sensor data at appropriate intervals
    if (currentTime - previousTime >= interval) {
        Serial.print("acceleration is ");
        Serial.print(acceleration);
        Serial.print("recorded at ");
        Serial.print(time_convert(accelerationReadTime));
        previousTime = millis();

        //Temporary place holder for acceleration value
        acceleration += 1;

        //Logs the data to the SD card module
        logData(acceleration, currentTime);

        //Closes the file
        if(closeFile == true){
            file.close();
            Serial.println(F("Done"));
        }

    }

}
