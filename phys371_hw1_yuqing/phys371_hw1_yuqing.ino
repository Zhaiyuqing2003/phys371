/*
Please provide us with the following information:

  your name: Yuqing Zhai
  
  which homeork assignment/problem this code is for: Homework 1
  
  today's date: Sep. 1st 2022
  
  name of this file: phys371_hw1_yuqing

  goal/purpose of this program: read the data from the bme680, and display them on LCD
  
  approximate time spent on this assignment/problem: 7 hours

  students with whom you worked while developing this code: N/A
  
  references you used in developing your code: 
    Example > Adafruit BME680 Library > bme680test
    Arduino Documentation : https://docs.arduino.cc/learn/electronics/lcd-displays
*/

//////////////////////////////////////////////////////////////////////////////////////

// global variables (which are known to all functions) can go here.
// headers included
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
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

//////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  // this function is executed once, immediately upon initiation of the program.
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  // initialize lcd
  lcd.begin(16, 2);
}

//////////////////////////////////////////////////////////////////////////////////////

void loop() 
{
  // this function is executed repeatedly.
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // NOTE: since the lcd only contain 16 * 2 = 32 character display,
  // it's hard to display all information at once
  // a possible solution is to use lcd.autoscroll() function to
  // to achieve the display of all information via scrolling.
  // but since auto scrolling could only scroll the information at a
  // speed of 500ms per tile, it results in the information
  // from bme680 only update once in 5 - 10 seconds,
  // so eventualy, the solution is to only display two informations on
  // the lcd board, leaving other informations only print out on the
  // serials. If you want to change the informations print on the lcd,
  // uncomment some line accordingly. 

  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  lcd.setCursor(0, 0);
  lcd.print("T = ");
  lcd.print(bme.temperature);
  lcd.print(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  lcd.setCursor(0, 1);
  lcd.print("P = ");
  lcd.print(bme.pressure / 100.0);
  lcd.print(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

//  lcd.setCursor(0, 0);
//  lcd.print("Humidity= ");
//  lcd.print(bme.humidity);
//  lcd.print("%");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

//  lcd.setCursor(0, 1);
//  lcd.print("Gas=");
//  lcd.print(bme.gas_resistance / 1000.0);
//  lcd.print("KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

//  lcd.setCursor(0, 0);
//  lcd.print("Altitude=");
//  lcd.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//  lcd.print("m");

  Serial.println();
}

//////////////////////////////////////////////////////////////////////////////////////

// other functions can go here.

//////////////////////////////////////////////////////////////////////////////////////
