/**************************************************************************
simple_logging_example.ino
Initial Creation Date: 6/3/2016
Written By:  Jeff Horsburgh (jeff.horsburgh@usu.edu)
Updated By:  Kenny Fryar-Ludwig (kenny.fryarludwig@usu.edu)
Additional Work By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Radio Module: Sodaq GPRSbee or Digi XBee S6B (WiFi)
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team
This sketch is an example of posting data to the Web Streaming Data Loader
Assumptions:
1. The Bee WiFi module has must be configured correctly to connect to the
   wireless network prior to running this sketch. (If Applicable)
2. The Mayfly has been registered at http://data.envirodiy.org and the sensor
   has been configured. In this example, only temperature and battery voltage are used.
DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
**************************************************************************/

// -----------------------------------------------
// Note: All 'Serial.print' statements can be
// removed if they are not desired - used for
// debugging only
// -----------------------------------------------


// -----------------------------------------------
// 1. Include all sensors and necessary files here
// -----------------------------------------------
#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <SdFat.h>
#include <RTCTimer.h>
#include <Sodaq_DS3231.h>
#include <Sodaq_PcInt_PCINT0.h>
#include <DallasTemperature.h>
#include <OneWire.h>
const char *FILE_NAME="tempsting";
// Data wire is connected to the Arduino digital pin 6
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// -----------------------------------------------
// 4. Timing Options For Logging
// -----------------------------------------------
int LOGGING_INTERVAL = 1;  // How frequently (in minutes) to log data
int READ_DELAY = 1;  // How often (in minutes) the timer wakes up
int UPDATE_RATE = 200; // How frequently (in milliseconds) the logger checks if it should log
int COMMAND_TIMEOUT = 15000;  // How long (in milliseconds) to wait for a server response
const int TIME_ZONE = -0;
// -----------------------------------------------
// 6. Board setup info
// -----------------------------------------------
int SERIAL_BAUD = 9600;  // Serial port BAUD rate
int GREEN_LED = 8;  // Pin for the green LED
int RED_LED = 9;  // Pin for the red LED

int RTC_PIN = A7;  // RTC Interrupt pin
#define RTC_INT_PERIOD EveryMinute  //The interrupt period on the RTC

int BATTERY_PIN = A6;    // select the input pin for the potentiometer

int SD_SS_PIN = 12;  // SD Card Pin

// -----------------------------------------------
// 7. Setup variables
// -----------------------------------------------
float ONBOARD_TEMPERATURE = 0;  // Variable to store the temperature result in
float ONBOARD_BATTERY = 0;  // variable to store the value coming from the sensor

// Variables for the timer function
int currentminute;
int testtimer = 0;
int testminute = 1;
long currentepochtime = 0;

Sodaq_DS3231 sodaq;   // Controls the Real Time Clock Chip
RTCTimer timer;  // The timer functions for the RTC
SdFat SD;  // The SD initialization
String fileName = String(FILE_NAME);  // For the file name


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


String header="Time,batt_V,0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,6,7";

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

// -----------------------------------------------
// 8. Working functions
// -----------------------------------------------

// Helper function to get the current date/time from the RTC
// as a unix timestamp - and apply the correct time zone.
uint32_t getNow()
{
  currentepochtime = rtc.now().getEpoch();
  currentepochtime += TIME_ZONE*3600;
  return currentepochtime;
}

// This function returns the datetime from the realtime clock as an ISO 8601 formated string
String getDateTime_ISO8601(void)
{
  String dateTimeStr;
  //Create a DateTime object from the current time
  DateTime dt(rtc.makeDateTime(getNow()));
  //Convert it to a String
  dt.addToString(dateTimeStr);
  dateTimeStr.replace(F(" "), F("T"));
  String tzString = String(TIME_ZONE);
  if (-24 <= TIME_ZONE && TIME_ZONE <= -10)
  {
      tzString += F(":00");
  }
  else if (-10 < TIME_ZONE && TIME_ZONE < 0)
  {
      tzString = tzString.substring(0,1) + F("0") + tzString.substring(1,2) + F(":00");
  }
  else if (TIME_ZONE == 0)
  {
      tzString = F("Z");
  }
  else if (0 < TIME_ZONE && TIME_ZONE < 10)
  {
      tzString = "+0" + tzString + F(":00");
  }
  else if (10 <= TIME_ZONE && TIME_ZONE <= 24)
  {
      tzString = "+" + tzString + F(":00");
  }
  dateTimeStr += tzString;
  return dateTimeStr;
}

// This sets up the function to be called by the timer with no return of its own.
// This structure is required by the timer library.
// See http://support.sodaq.com/sodaq-one/adding-a-timer-to-schedule-readings/
void showTime(uint32_t ts)
{
  // Retrieve the current date/time
  String dateTime = getDateTime_ISO8601();
  return;
}

// Set-up the RTC Timer events
void setupTimer()
{
  // This tells the timer how often (READ_DELAY) it will repeat some function (showTime)
  timer.every(READ_DELAY, showTime);

  // Instruct the RTCTimer how to get the current time reading (ie, what function to use)
  timer.setNowCallback(getNow);
}

void wakeISR()
{
  //Leave this blank
}


// Sets up the sleep mode (used on device wake-up)
void setupSleep()
{
  pinMode(RTC_PIN, INPUT_PULLUP);
  PcInt::attachInterrupt(RTC_PIN, wakeISR);

  //Setup the RTC in interrupt mode
  rtc.enableInterrupts(RTC_INT_PERIOD);

  //Set the sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}


// Puts the system to sleep to conserve battery life.
void systemSleep()
{
  // Wait until the serial ports have finished transmitting
  Serial.flush();
  Serial1.flush();

  // The next timed interrupt will not be sent until this is cleared
  rtc.clearINTStatus();

  // Disable ADC
  ADCSRA &= ~_BV(ADEN);

  // Sleep time
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();
}

// Initializes the SDcard and prints a header to it
void setupLogFile()
{
  // Initialise the SD card
  if (!SD.begin(SD_SS_PIN))
  {
    Serial.println(F("Error: SD card failed to initialise or is missing."));
  }

  fileName += "_" + getDateTime_ISO8601().substring(0,10) + ".csv";

  // Open the file in write mode
  File logFile = SD.open(fileName, FILE_WRITE);
  logFile.println(header);

  //Close the file to save it
  logFile.close();
}

// Flashess to Mayfly's LED's
void greenred4flash()
{
  for (int i = 1; i <= 4; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(50);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(50);
  }
  digitalWrite(RED_LED, LOW);
}

// This function updates the values for any connected sensors. Need to add code for
// Any sensors connected - this example only uses temperature and battery voltage.
bool updateAllSensors()
{
    // Get the temperature from the Mayfly's real time clock
   // rtc.convertTemperature();  //convert current temperature into registers
    //float tempVal = rtc.getTemperature();
    //ONBOARD_TEMPERATURE = tempVal;

    // Get the battery voltage
    float rawBattery = analogRead(BATTERY_PIN);
    ONBOARD_BATTERY = (3.3 / 1023.) * 1.47 * rawBattery;

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
    return true;
}

String dataString(void)
{
String datastring = getDateTime_ISO8601()+","+String(ONBOARD_BATTERY)+ ","+ String(Sensor00) + "," + String(Sensor050) + "," + String(Sensor0100) + "," + String(Sensor0150) + "," + 
String(Sensor0200) + "," + String(Sensor0250) + "," + String(Sensor0300) + "," + String(Sensor0350) + "," + 
String(Sensor0400) + "," + String(Sensor0450) + "," + String(Sensor0500) + "," + String(Sensor0600) + "," + 
String(Sensor0700);
return datastring;
}

// Writes a string to a text file on the SD Card
void logData(String dataString)
{
  // Re-open the file
  File logFile = SD.open(fileName, FILE_WRITE);

  // Write the CSV data
  logFile.println(dataString);

  // Close the file to save it
  logFile.close();
}



void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);
 
    // Start the Real Time Clock
    rtc.begin();
    delay(100);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Set up the log file
    setupLogFile();

    // Setup timer events
    setupTimer();

    // Setup sleep mode
    setupSleep();

    // Print a start-up note to the first serial port
    Serial.print(F("Current Mayfly RTC time is: "));
    Serial.println(getDateTime_ISO8601());
}

void loop()
{
 
    // Update the timer
    timer.update();

    // Check of the current time is an even interval of the test minute
    if (currentminute % testminute == 0)
    {
        // Turn on the LED
        digitalWrite(GREEN_LED, HIGH);
        // Print a few blank lines to show new reading
        Serial.println(F("\n---\n---\n"));
        // Get the sensor value(s), store as string
        updateAllSensors();
        //Save the data record to the log file
        logData(dataString());
        // Turn off the LED
        Serial.println(dataString());
        digitalWrite(GREEN_LED, LOW);
        // Advance the timer

        testtimer++;
    }

    // Check if the time is an even unit of the logging interval
    if (testtimer >= LOGGING_INTERVAL)
    {
       testminute = LOGGING_INTERVAL;
    }
       sensors.begin();



    // Sleep
    delay(UPDATE_RATE);
    systemSleep();
}
