// Falling Object Group 4, Based on George Gollin (Professor) code.

// Headers
#include <SD.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>  
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_H3LIS331.h>


///////////////////////// LSM9DS1 parameters ///////////////////////
// PINS
#define LSM9DS1_XGCS 12
#define LSM9DS1_MCS 11

// CONSTS
const int LSM9DS1_SAMPLE_INTERVAL = 1051;
int LSM9DS1_SATURATION = 0;

// SPI
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);

// Time counter
uint32_t lsm_last_sampled_time = 0;

///////////////////////// H3LIS331 parameters //////////////////////
// PINS
#define H3LIS331_CS 9
#define H3LIS331_SCK 24
#define H3LIS331_MISO 22
#define H3LIS331_MOSI 23

// CONSTS
const int H3LIS331_SAMPLE_INTERVAL = 1000;
const int H3LIS331_SATURATION = 15.99 * G_EARTH;

// SPI
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

// Time counter
uint32_t lis_last_sampled_time = 0;

//////////////////////// SD card parameters /////////////////////////
#define SD_PIN 4

File file;
const char FILE_NAME[10] = "FALL.TXT";

// buffers for non-blocking buffer
char buffer[512];

//////////////////////// GLOBALS ////////////////////////////////////

// for storing 
struct data {
  double sum;
  double sum_sq;
  double rms;
};

struct offset {
  double ax, ay, az;
};

// CONSTANTS
const bool G_EARTH = 9.8067;
const int LED_GREEN_PIN = 8;
const int LED_RED_PIN = 13;

void setup() {
  Serial.begin(115200);

  uint32_t start = millis();
  // wait for 10 seconds for Serial otherwise just keep going
  while (!Serial && millis() - time_start < 10000) delay(1);

  blink(5000);

  setup_sd();
  setup_lsm();


}

void loop() {
  // put your main code here, to run repeatedly:

}

void setup_lis() {
  if (!lis.begin_SPI(H3LIS331_CS)) {
    Serial.println("Unable to Initialize H3LIS331, Exiting...");
    while (1) yield();
  }

  lis.setRange(H3LIS331_RANGE_400_G);
  lis.setDataRate(LIS331_DATARATE_1000_HZ);

}

void setup_lsm() {
  if (!lsm.begin()) {
    Serial.println("Unable to Initialize LSM9DS1, Exiting...");
    while (1);
  }

  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

void setup_sd() {
  if (!card.init(SPI_FULL_SPEED, SD_PIN)) {
    Serial.println("Unable to Initialize SD, Exiting...");
    while (1);
  }

  SD.begin(SD_PIN);
}

void blink(uint32_t duration) {
  Serial.print("Blinking green and red lights for ");
  Serial.print(duration);
  Serial.print(" seconds");

  uint32_t start = millis();
  int interval = 50;

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  while (millis() - start < duration) {
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);

    delay(interval);

    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, HIGH);

    delay(interval);
  }

  // restore the led
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
}
