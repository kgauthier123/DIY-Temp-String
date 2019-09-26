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
#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#define interruptPin 0 //Pin we are going to use to wake up the Arduino
#include "SD.h"
#include"SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <DS3232RTC.h>
RTC_DS3231 rtc;
const int time_interval=1;

// Data wire is connected to the Arduino digital pin 6
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
DeviceAddress Sensor0 = {0x28, 0x75, 0xA4, 0xFC, 0xA, 0x0, 0x0, 0x48};
DeviceAddress Sensor50 = {0x28, 0xF7, 0x15, 0x13, 0xB, 0x0, 0x0, 0x36};
DeviceAddress Sensor100 = {0x28, 0xD6, 0x3F, 0xFD, 0xA, 0x0, 0x0, 0x5};
DeviceAddress Sensor150 = {0x28, 0x3F, 0xB0, 0xFC, 0xA, 0x0, 0x0, 0x91};
DeviceAddress Sensor200 = {0x28, 0xDA, 0x83, 0xFC, 0xA, 0x0, 0x0, 0x99};
DeviceAddress Sensor250 = {0x28, 0xE4, 0xD, 0xFD, 0xA, 0x0, 0x0, 0x81};
DeviceAddress Sensor300 = {0x28, 0x69, 0x64, 0xFD, 0xA, 0x0, 0x0, 0xC3};
DeviceAddress Sensor350 = {0x28, 0xA7, 0x1D, 0xFD, 0xA, 0x0, 0x0, 0xD1};
DeviceAddress Sensor400 = {0x28, 0x8A, 0x94, 0xFC, 0xA, 0x0, 0x0, 0x9A};
DeviceAddress Sensor450 = {0x28, 0xCA, 0x6B, 0xFC, 0xA, 0x0, 0x0, 0x26};
DeviceAddress Sensor500 = {0x28, 0x40, 0x47, 0xFC, 0xA, 0x0, 0x0, 0xDA};
DeviceAddress Sensor600 = {0x28, 0xE4, 0xD2, 0xFC, 0xA, 0x0, 0x0, 0x3F};
DeviceAddress Sensor700 = {0x28, 0x7F, 0x81, 0xFD, 0xA, 0x0, 0x0, 0x22};

String header="Time,0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,6,7";
//the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD
// library functions will not work.
const int CSpin = 4;
String dataString =""; // holds the data to be written to the SD card
float Sensor00 = 0.00; // value read from your first sensor
float Sensor050 = 0.00; // value read from your second sensor
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
float Sensor0700 = 0.00;

File sensorData;
//
//
void setup()
{
// Open serial communications
Serial.begin(9600);
  //pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  //digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);
    
    time_t t; //create a temporary time variable so we can set the time and read the time from the RTC
    t=RTC.get();//Gets the current time of the RTC
    RTC.setAlarm(ALM1_MATCH_SECONDS , 0, minute(t)+time_interval, 0, 0);// Setting alarm 1
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 1
    RTC.alarmInterrupt(ALARM_1, true);
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
Sensor050=(sensors.getTempC(Sensor50));
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
Sensor0700=(sensors.getTempC(Sensor700));

// build the data string
dataString = String(Sensor00) + "," + String(Sensor050) + "," + String(Sensor0100) + "," + String(Sensor0150) + "," + 
String(Sensor0200) + "," + String(Sensor0250) + "," + String(Sensor0300) + "," + String(Sensor0350) + "," + 
String(Sensor0400) + "," + String(Sensor0450) + "," + String(Sensor0500) + "," + String(Sensor0600) + "," + 
String(Sensor0700); // convert to CSV

getTime();
saveData(); // save to SD card
Serial.println(dataString);
//delay(3000);
Going_To_Sleep();    
 // delay before next write to SD Card, adjust as required
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
    /*Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();*/
    
    if (sensorData){
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
}}

void Going_To_Sleep(){
  //Serial.println("Going to sleep");
  sleep_enable();
  attachInterrupt(2, wakeUp, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //digitalWrite(LED_BUILTIN,LOW);
  time_t t;// creates temp time variable
  t=RTC.get(); //gets current time from rtc
  Serial.println("Sleep  Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//prints time stamp on serial monitor
  //delay(1000);
  sleep_cpu();
  //Serial.println("Just woke up!");
  //digitalWrite(LED_BUILTIN,HIGH);
  t=RTC.get();
  Serial.println("WakeUp Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//Prints time stamp 
  RTC.setAlarm(ALM1_MATCH_MINUTES , 0, minute(t)+time_interval, 0, 0);//Set New Alarm
  RTC.alarm(ALARM_1);  // clear the alarm flag
}

void wakeUp(){
  Serial.println("Interrupt Fired");
  sleep_disable();
  detachInterrupt(2);
}
