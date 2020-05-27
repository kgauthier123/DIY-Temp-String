#include <avr/sleep.h>
#include "SD.h"
#include "SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <DS3232RTC.h>

RTC_DS3231 rtc;
const int time_interval=1;

// Data wire is connected to the Arduino digital pin 6
#define ONE_WIRE_BUS 6

// Specify pin to read batttery voltage from
#define VBATPIN A9

// Specify pin we are going to use to wake up the Arduino
#define interruptPin 0 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
// Format: Sensor(depth in cm) = {Unique serial number}
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

// Create a string of column headers to be written to .csv file
String header="Time,Voltage,0,0.25,0.5,0.75,1,1.5,2,2.5,3,3.5,4,4.5,5,6";

const int CSpin = 4;

// Specify a string of the temperature data to be written to the SD card
String dataString =""; 

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


void setup()
{
// Open serial communications
Serial.begin(9600);

pinMode(interruptPin,INPUT_PULLUP);

// Initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
RTC.alarm(ALARM_1);
RTC.alarm(ALARM_2);
RTC.alarmInterrupt(ALARM_1, false);
RTC.alarmInterrupt(ALARM_2, false);
RTC.squareWave(SQWAVE_NONE);

// Create a temporary time variable so we can set the time and read the time from the RTC
time_t t; 
    
// Get the current time of the RTC    
t=RTC.get();

// Set alarm 2 to occur once per minute
RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0);

// clear the alarm flag
RTC.alarm(ALARM_2);
    
// enable interrupt output for Alarm 2
RTC.alarmInterrupt(ALARM_2, true);
}


//------------------------------------------------------------------------
// Function to check for an existing data file on SD card, if none present 
//  create the file and write the column headers
//------------------------------------------------------------------------
void Initialize(){
  
pinMode(CSpin, OUTPUT);
sensors.begin();

// Print message to serial monitor if no SD card is present
if (!SD.begin(CSpin)) {
Serial.println("Card failed, or not present");
return;
}
  
// Check for file and write with header if absent
if(SD.exists("data.csv")){
  Serial.println("File exists");
}
else{
  Serial.println("No file, writing new");
  sensorData = SD.open("data.csv", FILE_WRITE);
  sensorData.println(header);
  sensorData.close();
}}


//------------------------------------------
// Function for logging time stamp using RTC
//------------------------------------------
void getTime(){
  sensorData = SD.open("DATA.CSV", FILE_WRITE);
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
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
    Serial.println();
    
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
    sensorData.print(":00");
    sensorData.print(",");
    sensorData.close();
}}


//--------------------------------------------------
// Function for writing temperature data to SD card
//--------------------------------------------------
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


//-------------------------------------
// Function for putting system to sleep
//-------------------------------------
void Going_To_Sleep(){
  Serial.println("Going to sleep");
  sleep_enable();
  attachInterrupt(2, wakeUp, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  time_t t; //creates temp time variable
  t=RTC.get(); //gets current time from rtc
  sleep_cpu();

  t=RTC.get();
  RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0); //Set new alarm  
  RTC.alarm(ALARM_2);  // clear the alarm flag
}


//----------------------------------------
// Function for waking system up to sample
//----------------------------------------
void wakeUp(){
  Serial.println("Interrupt Fired");
  sleep_disable();
  detachInterrupt(2);
}


void loop(){

// Create file with header on the SD card
Initialize();

// Get updated values for all connected sensors
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

// Get battery voltage reading
float measuredvbat = analogRead(VBATPIN); //read voltage
measuredvbat *= 2;    // divided by 2 internally, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage

// Build a data string of temperatures from each sensor and battery voltage in .csv format
dataString = String(measuredvbat) + "," + String(Sensor00) + "," + String(Sensor025) + "," + String(Sensor050) + "," + String(Sensor075) + "," + String(Sensor0100) + "," + String(Sensor0150) + "," + 
String(Sensor0200) + "," + String(Sensor0250) + "," + String(Sensor0300) + "," + String(Sensor0350) + "," + 
String(Sensor0400) + "," + String(Sensor0450) + "," + String(Sensor0500) + "," + String(Sensor0600);

// Get time stamp from RTC
getTime();

// Write temperature data to SD card
saveData();

// Print all data to serial monitor if desired
Serial.println(dataString);

// Put board to sleep until next sample
Going_To_Sleep();    
}
