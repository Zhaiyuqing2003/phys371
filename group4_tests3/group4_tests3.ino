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

const double G_EARTH = 9.8067;

// CONSTS
const int LSM9DS1_SAMPLE_INTERVAL = 1051; // in micros (frequency = 952 Hz)
const double LSM9DS1_SATURATION = 15.9 * G_EARTH;

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
const int H3LIS331_SAMPLE_INTERVAL = 1000; // in micros (frequency = 1000 Hz)
const double H3LIS331_SATURATION = 15 * G_EARTH;
// SPI
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

// Time counter
uint32_t lis_last_sampled_time = 0;

//////////////////////// GLOBALS ////////////////////////////////////

// for storing 
struct offset_t {
  double ax = 0.0, ay = 0.0, az = 0.0;
  double ax_rms = 0.0, ay_rms = 0.0, az_rms = 0.0;
};

struct data_t {
  unsigned long time = 0UL;
  float ax_lsm = 0.0f, ay_lsm = 0.0f, az_lsm = 0.0f;
  float ax_lis = 0.0f, ay_lis = 0.0f, az_lis = 0.0f;
};

// CONSTANTS
const int LED_GREEN_PIN = 8;
const int LED_RED_PIN = 13;
const int LOOPS = 300000; // 5 minutes

//////////////////////// SD card parameters /////////////////////////
#define SD_PIN 4


Sd2Card card;
File file;
File count_file;
String FILE_NAME;
const char COUNT_FILE_NAME[15] = "COUNT.TXT";

// buffers for non-blocking write
char buffer[sizeof(data_t) * 512];


//////////////////////// DECLARATIONS ///////////////////////////////
void open_file();
void offset_test(int);
offset_t offset_lsm_test(int);
offset_t offset_lis_test(int);
void speed_test();
void delay_lsm();
void delay_lis();
void setup_lis();
void setup_lsm();
void setup_sd();
void blink(uint32_t, int = 50);

void setup() {
  Serial.begin(115200);

  uint32_t start = millis();
  // wait for 10 seconds for Serial otherwise just keep going
  while (!Serial && millis() - start < 10000) delay(1);

  blink(5000);

  Serial.println("------------ Setting Up -------------");

  setup_sd();
  setup_lsm();
  setup_lis();

  // open file
  open_file();

  // offset test
  offset_test(5000);
  blink(5000, 100);
}

void loop() {
  static char* buffer_ptr = buffer;
  static int index = 0;
  static data_t data;

  unsigned long micros_now = micros();

  if (index > LOOPS) {
    // just close the file and stop
    file.close();
    while (1) {
      blink(10000, 100);
    }
  } else {
    index ++;
  }

  // // if (using_lsm) {
  sensors_event_t accel, mag, gyro, temp;
  delay_lsm();

  lsm.getEvent(&accel, &mag, &gyro, &temp);
  lsm.readAccel();

  data.time = micros_now;
  data.ax_lsm = float(accel.acceleration.x);
  data.ay_lsm = float(accel.acceleration.y);
  data.az_lsm = float(accel.acceleration.z);

  // } else {
  sensors_event_t accel_two;
  delay_lis();

  lis.getEvent(&accel_two);

  data.ax_lis = float(accel_two.acceleration.x);
  data.ay_lis = float(accel_two.acceleration.y);
  data.az_lis = float(accel_two.acceleration.z);

  memcpy(buffer_ptr, &data, sizeof(data_t));
  buffer_ptr += sizeof(data_t);

  // write to file
  // unsigned int chunkSize = file.availableForWrite();

  if (buffer_ptr - buffer >= sizeof(buffer)) {
    Serial.println("Writing to file... ");
    file.write(buffer, sizeof(buffer));
    file.close();
    file = SD.open(FILE_NAME, FILE_WRITE);
    buffer_ptr = buffer;
  }
}

void open_file() {
  Serial.println("---------- Opening Count File ----------");

  if (!SD.exists(COUNT_FILE_NAME)) {
    count_file = SD.open(COUNT_FILE_NAME, FILE_WRITE);
    count_file.print("0");
    count_file.close();
  }

  count_file = SD.open(COUNT_FILE_NAME);

  String count_string = "";

  if (count_file) {
    while (count_file.available()) {
      Serial.println("Am I here?");
      count_string += (char)count_file.read();
    }
  }

  int count = count_string.toInt();

  Serial.println(count_string);
  Serial.println(count);

  count_file.close();

  SD.remove(COUNT_FILE_NAME);
  count_file = SD.open(COUNT_FILE_NAME, FILE_WRITE);
  count_file.print(String(count + 1));
  count_file.close();

  Serial.println("---------- Opening File ----------");

  FILE_NAME = String("FILE") + count + ".TXT";
  
  if (SD.exists(FILE_NAME)) {
    Serial.println("File exists, deleting...");
    SD.remove(FILE_NAME);
  }

  file = SD.open(FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file");
    blink(1000, 150);
    while (1);
  }
}

void offset_test(int times) {
  Serial.println("---------- Calculating Offset -----------");
  
  auto offset_lsm = offset_lsm_test(times);
  auto offset_lis = offset_lis_test(times);

  file.write((char*) &offset_lsm, sizeof(offset_lsm));
  file.write((char*) &offset_lis, sizeof(offset_lis));
}

offset_t offset_lsm_test(int times) {
  Serial.println("Calculating LSM Offset...");

  sensors_event_t accel, mag, gyro, temp;

  offset_t lsm_offset {};

  for (int i = 0; i < times; i++) {
    delay_lsm();
    lsm.getEvent(&accel, &mag, &gyro, &temp);
    lsm.readAccel();

    lsm_offset.ax += accel.acceleration.x;
    lsm_offset.ay += accel.acceleration.y;
    lsm_offset.az += accel.acceleration.z;
    lsm_offset.ax_rms += accel.acceleration.x * accel.acceleration.x;
    lsm_offset.ay_rms += accel.acceleration.y * accel.acceleration.y;
    lsm_offset.az_rms += accel.acceleration.z * accel.acceleration.z;
  }

  lsm_offset.ax /= times;
  lsm_offset.ay /= times;
  lsm_offset.az /= times;
  lsm_offset.ax_rms /= times;
  lsm_offset.ay_rms /= times;
  lsm_offset.az_rms /= times;

  lsm_offset.ax_rms = sqrt(lsm_offset.ax_rms - lsm_offset.ax * lsm_offset.ax);
  lsm_offset.ay_rms = sqrt(lsm_offset.ay_rms - lsm_offset.ay * lsm_offset.ay);
  lsm_offset.az_rms = sqrt(lsm_offset.az_rms - lsm_offset.az * lsm_offset.az);

  // Az should minus the gravity
  lsm_offset.az -= G_EARTH;

  return lsm_offset;
}

offset_t offset_lis_test(int times) {
  Serial.println("Calculating LIS Offset...");

  sensors_event_t accel;

  offset_t lis_offset {};

  for (int i = 0; i < times; i++) {
    delay_lis();
    lis.getEvent(&accel);

    lis_offset.ax += accel.acceleration.x;
    lis_offset.ay += accel.acceleration.y;
    lis_offset.az += accel.acceleration.z;
    lis_offset.ax_rms += accel.acceleration.x * accel.acceleration.x;
    lis_offset.ay_rms += accel.acceleration.y * accel.acceleration.y;
    lis_offset.az_rms += accel.acceleration.z * accel.acceleration.z;
  }

  lis_offset.ax /= times;
  lis_offset.ay /= times;
  lis_offset.az /= times;
  lis_offset.ax_rms /= times;
  lis_offset.ay_rms /= times;
  lis_offset.az_rms /= times;

  lis_offset.ax_rms = sqrt(lis_offset.ax_rms - lis_offset.ax * lis_offset.ax);
  lis_offset.ay_rms = sqrt(lis_offset.ay_rms - lis_offset.ay * lis_offset.ay);
  lis_offset.az_rms = sqrt(lis_offset.az_rms - lis_offset.az * lis_offset.az);

  // Az should minus the gravity
  lis_offset.az -= G_EARTH;

  return lis_offset;
}

void speed_test() {

}

void delay_lsm() {
  while (micros() - lsm_last_sampled_time < LSM9DS1_SAMPLE_INTERVAL);
  lsm_last_sampled_time = micros();
}

void delay_lis() {
  while (micros() - lis_last_sampled_time < H3LIS331_SAMPLE_INTERVAL);
  lis_last_sampled_time = micros();
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

void blink(uint32_t duration, int interval) {
  Serial.print("------------ Blinking green and red lights for ");
  Serial.print(duration / 1000);
  Serial.print(" seconds ------------\n");

  uint32_t start = millis();

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
