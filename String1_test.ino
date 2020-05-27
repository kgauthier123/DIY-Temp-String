/*
Writing Sensor Data to an SD card
//
This example shows how to write data
to an SD card using the SD library.
//
The circuit:
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 10 Uno (53 on Mega)
Based on code by Tom Igoe
*/

#include "SD.h"
#include"SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>


// Data wire is connected to the Arduino digital pin 6
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
DeviceAddress Sensor0 = {0x28, 0x6C, 0x48, 0xBC, 0xB, 0x0, 0x0, 0x35};
DeviceAddress Sensor25 = {0x28, 0x46, 0x88, 0xBC, 0xB, 0x0, 0x0, 0x6E};
DeviceAddress Sensor50 = {0x28, 0x18, 0x61, 0xBE, 0xB, 0x0, 0x0, 0x7D};
DeviceAddress Sensor75 = {0x28, 0x6C, 0x27, 0xBC, 0xB, 0x0, 0x0, 0x4B};
DeviceAddress Sensor100 = {0x28, 0xFB, 0x0, 0xBB, 0xB, 0x0, 0x0, 0x50};
DeviceAddress Sensor150 = {0x28, 0x55, 0xDA, 0xBE, 0xB, 0x0, 0x0, 0x7D};
DeviceAddress Sensor200 = {0x28, 0x18, 0x18, 0xBD, 0xB, 0x0, 0x0, 0x6B};
DeviceAddress Sensor250 = {0x28, 0x74, 0x6C, 0xBD, 0xB, 0x0, 0x0, 0xA7};
DeviceAddress Sensor300 = {0x28, 0x66, 0x45, 0xFC, 0xA, 0x0, 0x0, 0x5D};
DeviceAddress Sensor350 = {0x28, 0xC6, 0xA4, 0xBB, 0xB, 0x0, 0x0, 0xDB};
DeviceAddress Sensor400 = {0x28, 0xFE, 0x57, 0xBD, 0xB, 0x0, 0x0, 0x76};
DeviceAddress Sensor450 = {0x28, 0x72, 0x8, 0xBB, 0xB, 0x0, 0x0, 0x12};
DeviceAddress Sensor500 = {0x28, 0x1C, 0x62, 0xBD, 0xB, 0x0, 0x0, 0x67};
DeviceAddress Sensor600 = {0x28, 0x39, 0xDC, 0xBD, 0xB, 0x0, 0x0, 0xD7};

String header="Time,0,0.25,0.5,0.75,1,1.5,2,2.5,3,3.5,4,4.5,5,6";
//the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD
// library functions will not work.
const int CSpin = 4;
String dataString =""; // holds the data to be written to the SD card
float Sensor00 = 0.00;
float Sensor025 = 0.00;
float Sensor050 = 0.00;
float Sensor075 = 0.00;
float Sensor0100 = 0.00;
float Sensor0150 = 0.00;
float Sensor0200 = 0.00;
float Sensor0250 = 0.00;
float Sensor0300 = 0.00;
float Sensor0350 = 0.00;
float Sensor0400 = 0.00;
float Sensor0450 = 0.00;
float Sensor0500 = 0.00;
float Sensor0600 = 0.00;

File sensorData;
//
//
void setup()
{
// Open serial communications
Serial.begin(9600);
}
void Initialize(){
pinMode(CSpin, OUTPUT);
//
sensors.begin();
// see if the card is present and can be initialized:
if (!SD.begin(CSpin)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
  
//check for file and write with header if absent
if(SD.exists("data.csv")){
  Serial.println("File exists");
}
else{
  Serial.println("No file, writing new");
  sensorData = SD.open("data.csv", FILE_WRITE);
  sensorData.println(header);
  sensorData.close();
}
}

void loop(){
Initialize();
sensors.requestTemperatures(); 
Sensor00=(sensors.getTempC(Sensor0));
Sensor025=(sensors.getTempC(Sensor25));
Sensor050=(sensors.getTempC(Sensor50));
Sensor075=(sensors.getTempC(Sensor75));
Sensor0100=(sensors.getTempC(Sensor100));
Sensor0150=(sensors.getTempC(Sensor150));
Sensor0200=(sensors.getTempC(Sensor200));
Sensor0250=(sensors.getTempC(Sensor250));
Sensor0300=(sensors.getTempC(Sensor300));
Sensor0350=(sensors.getTempC(Sensor350));
Sensor0400=(sensors.getTempC(Sensor400));
Sensor0450=(sensors.getTempC(Sensor450));
Sensor0500=(sensors.getTempC(Sensor500));
Sensor0600=(sensors.getTempC(Sensor600));

// Build a data string of temperatures from each sensor in .csv format
dataString = String(Sensor00) + "," + String(Sensor025) + "," + String(Sensor050) + "," + String(Sensor075) + "," + String(Sensor0100) + "," + String(Sensor0150) + "," + 
String(Sensor0200) + "," + String(Sensor0250) + "," + String(Sensor0300) + "," + String(Sensor0350) + "," + 
String(Sensor0400) + "," + String(Sensor0450) + "," + String(Sensor0500) + "," + String(Sensor0600);

saveData(); // save to SD card
Serial.println(dataString);    
delay(5000); // delay before next write to SD Card, adjust as required
}

//
void saveData(){
if(SD.exists("data.csv")){ // check the card is still there
// now append new data file
sensorData = SD.open("data.csv", FILE_WRITE);
if (sensorData){
sensorData.println(dataString);
sensorData.close();
}
}
else{Serial.println("Error writing to file !");}
}
