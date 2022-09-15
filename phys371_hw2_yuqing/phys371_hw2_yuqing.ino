/*
Please provide us with the following information:

  your name: Yuqing Zhai
  
  which homeork assignment/problem this code is for: Homework 2
  
  today's date: Sep. 8st 2022
  
  name of this file: phys371_hw2_yuqing

  goal/purpose of this program: 
    (1) display the input of keypad on lcd (except #)
    (2) display time from RST on keypad when receive ** input 
    (3) write (time, line number) pair to sd card when receive *** input
    (4) end the program when receive **** input
    (5) notice, the (#) is used like "Enter" on keyboard, it could not be displayed on the keyboard
  
  approximate time spent on this assignment/problem: 17 hours

  students with whom you worked while developing this code: N/A
  
  references you used in developing your code: 
    Example > Adafruit BME680 Library > bme680test
    Arduino Documentation : https://docs.arduino.cc/learn/electronics/lcd-displays
    Example > Adafruit Keypad > keypad_test
    Example > SdFat > ExampleV1 > ReadWrite
    Example > RTClib > toString
    Class Webpage > Repository > File "Here is the code I used to check the PCBs before distributing them"
*/

//////////////////////////////////////////////////////////////////////////////////////

// global variables (which are known to all functions) can go here.
// headers included
#include <SPI.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>

#include "SdFat.h"
#include "RTClib.h"
#include "Adafruit_BME680.h"

// macros for bme680 pins
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

// initialize bme object
Adafruit_BME680 bme;

// initialize lcd object
// RS, EN, D4, D5, D6, D7 pin respectively
LiquidCrystal lcd(12, 11, 36, 34, 32, 30);

/// Keypad Initialization
const byte ROWS = 4; 
const byte COLS = 3;

// keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte Arduino_colPins[COLS] = {2, 3, 18}; 
byte Arduino_rowPins[ROWS] = {31, 33, 35, 37};

Keypad kpd = Keypad( makeKeymap(keys), Arduino_rowPins, Arduino_colPins, ROWS, COLS );

/// RTC initialization

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/// SD reader initialization
SdFat SD;
const uint8_t SD_CS_PIN = SS;
File myFile;


/// Set some global state;
String line = "";
bool shouldFlash = false;
bool isEnd = false;

//////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  // this function is executed once, immediately upon initiation of the program.
  Serial.begin(9600);
  while (!Serial);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    return;
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // let's not include the logic when rtc lost its power.

  // initialize lcd
  lcd.begin(16, 2);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Initialization failed!");
    return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////

void loop() 
{
  if (isEnd) return;
  char key = kpd.getKey();
  if (key) {
    if (key == '#') {
      Serial.print("Current line is: ");
      Serial.println(line);

      if (line == "**") {
          // log the rtc time to Serial first
          DateTime now = rtc.now();

          Serial.print(now.year(), DEC);
          Serial.print('/');
          Serial.print(now.month(), DEC);
          Serial.print('/');
          Serial.print(now.day(), DEC);
          Serial.print(" (");
          Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
          Serial.print(") ");
          Serial.print(now.hour(), DEC);
          Serial.print(':');
          Serial.print(now.minute(), DEC);
          Serial.print(':');
          Serial.print(now.second(), DEC);
          Serial.println();

          lcd.clear();
          // then to the lcd
          lcd.setCursor(0, 0);
          lcd.print(String(now.year()).substring(2));
          lcd.print('/');
          lcd.print(now.month());
          lcd.print('/');
          lcd.print(now.day());
          lcd.print(' ');
          lcd.print(now.hour());
          lcd.print(':');
          lcd.print(now.minute());
          lcd.print(':');
          lcd.print(now.second());

          lcd.setCursor(0, 1);
          lcd.print("continue...");

          line = "";
          shouldFlash = true;
          
      } else if (line == "***") {
        // write to line to sd card
        myFile = SD.open("homework.csv", FILE_WRITE);
        if (myFile) {
          Serial.print("writing to homework.csv");
          for (auto i = 0; i < 10; i++) {
              DateTime now = rtc.now();
    
              myFile.print(now.year(), DEC);
              myFile.print('/');
              myFile.print(now.month(), DEC);
              myFile.print('/');
              myFile.print(now.day(), DEC);
              myFile.print(" (");
              myFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
              myFile.print(") ");
              myFile.print(now.hour(), DEC);
              myFile.print(':');
              myFile.print(now.minute(), DEC);
              myFile.print(':');
              myFile.print(now.second(), DEC);
              myFile.print(" line number is: ");
              myFile.print(i);
              
              myFile.println();
          }
          myFile.close();
          // output the indication to lcd
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("write complete");
          lcd.setCursor(0, 1);
          lcd.print("continue...");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("write failed");
          lcd.setCursor(0, 1);
          lcd.print("continue...");
        }

        line = "";
        shouldFlash = true;
      } else if (line == "****") {
        // that's the end of the program,
        // arduino will no longer respond.
        line = "";
        shouldFlash = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("end of program!");

        isEnd = true;
        
      } else {
        line = "";
        shouldFlash = true;
      }
      
    
    } else {
      if (shouldFlash) {
        lcd.clear();
        shouldFlash = false;
      }
      lcd.print(key);
      line += key;
    }
  }

//  DateTime now = rtc.now();
//
//  Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.day(), DEC);
//  Serial.print(" (");
//  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//  Serial.print(") ");
//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
//  Serial.println();

}

//////////////////////////////////////////////////////////////////////////////////////

// other functions can go here.

//////////////////////////////////////////////////////////////////////////////////////
