//Sketch for creating a new file and storing sensor data with a timestamp 
//FOR ONE SENSOR
//Latency for SD card stands at around 668kb/sec, just well within limit
//Everytime that data is seperated by comma it is moved to the next column in the
//print.ln creates a new row
#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include "SdFat.h"



//variable initialization

//name of the CSV file prefix, keep it four character leave as Data
#define FILE_BASE_NAME "Data" 
char fileName[13] = FILE_BASE_NAME "00.csv";
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;

// Creates an sd object.
SdFat sd;
// Creates a file object
SdFile file;
//Variable to store accelaration values
int Accelaration = 0;
//Bool for file closing
bool closeFile = false;

//Function used to write the headers of the CSV file
void writeHeader(String column1) {
  file.print("Time of collection for " + column1);
  file.print(F(","));
  file.print(column1);
  file.println();
}
  
//Function used to input data into the CSV file, call the function and pass the actual sensor data and the time it was recorded
void logData(int data1, int timestamp1)
{
file.print(data1);
file.print(F(","));
file.print(timestamp1);
file.println();
}

void fileClose() {
closeFile = true;
}

void setup() {

  Serial.begin(9600);
  // Set the interrupt pin as input with pull-up resistor
  pinMode(2, INPUT_PULLUP); 
  // Attach the interrupt
  attachInterrupt(digitalPinToInterrupt(2), fileClose, CHANGE); 


  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Initializes SD card to make sure everything is working
  if (!sd.begin(10,  SD_SCK_MHZ(50))) {
    Serial.println("initialization failed. Things to check:");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  //Checks to see if file name exists, if it already does then it will make 
  //modifications to the fileName variable and change it to new one by adding numbers to the end of it
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } 
  }
  
  //Actually opens/creates a file with the fileName variable as the name of it
  if (!file.open(fileName, O_CREAT | O_WRITE | O_APPEND)) {
    Serial.print("Error opening ");
    Serial.println(fileName);
    return;
  }
  

  //Prints to serial Monitor what the name of the file the module is logging to
  Serial.print(F("Logging to: "));
  Serial.println(fileName);
  writeHeader("Acceleration");
  Serial.println(F("Press button to stop code"));

}

void loop() {
  //Interval at which the data is being logged to module, but have to change this to log everytime a CAN message comes in
  delay(50);
  //Variable i represents accelaration "data" for rn
  Accelaration += 1;

  int timestamp = millis();
  
  logData(Accelaration, timestamp);
  Serial.println(Accelaration);
  
  //Closes file a key on the desktop is pressed, change to interrupt with button
  if (closeFile == true) {
    file.close();
    Serial.println(F("Done"));
    while (true) {}
  }

}
