/*
Please provide us with the following information:

  your name: Yuqing Zhai
  
  which homework assignment/problem this code is for: Homework 3
  
  today's date: Sep. 15st 2022
  
  name of this file: phys371_hw3_yuqing

  goal/purpose of this program:
    (1) measure acceleration & force
    (2) output the data to SD card and serial.
  
  approximate time spent on this assignment/problem: 8 hours

  students with whom you worked while developing this code: N/A
  
  references you used in developing your code: 
    https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout.pdf
    https://arduinogetstarted.com/tutorials/arduino-force-sensor
*/

//////////////////////////////////////////////////////////////////////////////////////

// global variables (which are known to all functions) can go here.
// headers included
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>

#define cardSelect 4

///////////////// Accelerometer ////////////////////

// hardware SPI!
#define LSM9DS1_XGCS 5
#define LSM9DS1_MCS 6

Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);

//////////////// SD card ////////////////////

const char filename[] = "output.txt";
File outFile;

// non-blocking!
String buffer;

//////////////// Force Sensors /////////////

int fsrPin = 3; // A3 receive the voltage
double resistance = 10200.0; // 10.2k resistor;

//////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  buffer.reserve(16384);

  // LED for writing indication
  pinMode(LED_BUILTIN, OUTPUT);

  if (!lsm.begin()) {
    Serial.println("Unable to initialize LSM9DS1!");
    while (1);
  }

  setupAccelerometer();

  // init the SD card
  if (!SD.begin(cardSelect)) {
    Serial.println("Card Initialization Failed!");
    while (1);
  }

  outFile = SD.open(filename, FILE_WRITE);
  if (!outFile) {
    Serial.print("error opening");
    Serial.println(filename);
    while (1);
  }

  Serial.println("I am here!");
  digitalWrite(LED_BUILTIN, HIGH);
}

//////////////////////////////////////////////////////////////////////////////////////

void loop() 
{
  // current time;
  unsigned long now = micros();
  
  // first, get accelerometer data!
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);

  // log accle to Serial and SD card 
  auto ax = a.acceleration.x;
  auto ay = a.acceleration.y;
  auto az = a.acceleration.z;

  // Serial.print(now);
  // Serial.print(" ");
  // Serial.print(" ax: ");
  // Serial.print(ax);
  // Serial.print(" ay: ");
  // Serial.print(ay);
  // Serial.print(" az: ");
  // Serial.print(az);
  
  buffer += String(now) + " " + ax + " " + ay + " " + az + " \n";

  // now get the resistor reading 

  // double percentage = analogRead(fsrPin) / 1023.0;
  
  // FSR = R * (1 / p - 1)

  // if (percentage == 0) {
  //   Serial.print(" F: 0 N");
  //   buffer += "0";
  // } else {
  //   double fsr = resistance * (1 / percentage - 1);
  //   double fsr_conductance = 1000000.0 / fsr;
  //   double fsr_force = fsr_conductance <= 1000
  //     ? fsr_conductance / 80.0 
  //     : (fsr_conductance - 1000.0) / 30.0;

  //   Serial.print(" F:");
  //   Serial.print(fsr_force);
  //   buffer += fsr_force;
  // }

  // Serial.println();
  // buffer += '\n';

  unsigned int chunkSize = outFile.availableForWrite();
  if (chunkSize && buffer.length() >= chunkSize) {
    // digitalWrite(LED_BUILTIN, HIGH);
    outFile.write(buffer.c_str(), chunkSize);
    // digitalWrite(LED_BUILTIN, LOW);

    buffer.remove(0, chunkSize);
  }
}

//////////////////////////////////////////////////////////////////////////////////////

// other functions can go here.

void setupAccelerometer() {
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

//////////////////////////////////////////////////////////////////////////////////////
