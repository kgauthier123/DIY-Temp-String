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
//
#include "SD.h"
#include"SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;

// Data wire is connected to the Arduino digital pin 6
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

DeviceAddress sensorA = {0x28, 0xAA, 0x73, 0x97, 0x3F, 0x14, 0x1, 0xC1};
//DeviceAddress sensorB = {0x28, 0xAA, 0x28, 0x32, 0x3D, 0x14, 0x1, 0x43};
DeviceAddress sensorC = {0x28, 0xAA, 0xE1, 0x3D, 0x3C, 0x14, 0x1, 0xA3};

String header="Time,SensorA,SensorC";
//the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD
// library functions will not work.
const int CSpin = 4;
String dataString =""; // holds the data to be written to the SD card
float sensor1 = 0.00; // value read from your first sensor
//float sensor2 = 0.00; // value read from your second sensor
float sensor3 = 0.00; // value read from your third sensor
File sensorData;
//
//
void setup()
{
// Open serial communications
Serial.begin(9600);
while (!Serial);
Serial.println("Initializing SD card...");
pinMode(CSpin, OUTPUT);
//
sensors.begin();
// see if the card is present and can be initialized:
if (!SD.begin(CSpin)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");
  
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
//
void loop(){
sensors.requestTemperatures(); 
sensor1=(sensors.getTempC(sensorA));
//sensor2=(sensors.getTempC(sensorB));
sensor3=(sensors.getTempC(sensorC));

// build the data string
dataString = String(sensor1) + "," + String(sensor3); // convert to CSV
getTime();
saveData(); // save to SD card
Serial.println(dataString);    
delay(2000); // delay before next write to SD Card, adjust as required
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

void getTime(){
  sensorData = SD.open("DATA.CSV", FILE_WRITE);
    DateTime now = rtc.now();
    Serial.println("File opened");
    if (sensorData) {
      Serial.println("Writing to file");
    
    sensorData.print(now.year(), DEC);
    sensorData.print('/');
    sensorData.print(now.month(), DEC);
    sensorData.print('/');
    sensorData.print(now.day(), DEC);
    sensorData.print(" ");
    sensorData.print(now.hour(), DEC);
    sensorData.print(':');
    sensorData.print(now.minute(), DEC);
    sensorData.print(':');
    sensorData.print(now.second(), DEC);
    sensorData.print(",");
    sensorData.close();
}
delay(2000);
}
