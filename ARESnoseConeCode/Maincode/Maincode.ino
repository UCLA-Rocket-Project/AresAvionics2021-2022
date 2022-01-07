/*
 * Noted Bugs
 * -Do not put delay() commands in the loop. It prevents ICM from sampling.
 * -No need to pull the CS pin low for the ICM. No idea why it isn't necessary
 *      -Might be necessary when we add the GPS.
 * -It is necessary to pull the BMP390 pin up and down
 */


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Teensy-ICM-20948.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define SEALEVELPRESSURE_HPA (1013.25)
TeensyICM20948 icm20948;

TeensyICM20948Settings icmSettings =
{
  .cs_pin = 6,                  // SPI chip select pin
  .spi_speed = 50,          // SPI clock speed in Hz, max speed is 7MHz
  .mode = 1,                     // 0 = low power mode, 1 = high performance mode
  .enable_gyroscope = true,      // Enables gyroscope output
  .enable_accelerometer = true,  // Enables accelerometer output
  .enable_magnetometer = true,   // Enables magnetometer output
  .enable_quaternion = true,     // Enables quaternion output
  .gyroscope_frequency = 225,      // Max frequency = 225, min frequency = 1
  .accelerometer_frequency = 225,  // Max frequency = 225, min frequency = 1
  .magnetometer_frequency = 70,   // Max frequency = 70, min frequency = 1
  .quaternion_frequency = 50     // Max frequency = 225, min frequency = 50
};

#define BMP_CS 0
#define ICM_CS 6

Adafruit_BMP3XX bmp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(5000);

  if (! bmp.begin_SPI(BMP_CS)) {
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  //Set up ICM
  icm20948.init(icmSettings);
  //SPI.begin();
  //Set CS Pins
  pinMode(BMP_CS, OUTPUT);
  digitalWrite(BMP_CS, HIGH);
  pinMode(ICM_CS, OUTPUT);
  digitalWrite(ICM_CS, HIGH);
}

void loop() {
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  // put your main code here, to run repeatedly:
  float gyro_x, gyro_y, gyro_z;
  float accel_x, accel_y, accel_z;
  float mag_x, mag_y, mag_z;
  float quat_w, quat_x, quat_y, quat_z;
  char sensor_string_buff[128];
  
  //Toggle SPI for BMP
  digitalWrite(BMP_CS, LOW);
  
  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.print(" m.  ||  ");
  digitalWrite(BMP_CS, HIGH);
  //Toggle SPI for ICM
  
  icm20948.task();
  if (icm20948.gyroDataIsReady())
  {
    icm20948.readGyroData(&gyro_x, &gyro_y, &gyro_z);
    sprintf(sensor_string_buff, "Gyro (deg/s): [%f,%f,%f]", gyro_x, gyro_y, gyro_z);
    Serial.print(sensor_string_buff);
    Serial.print("  ||  ");
  }

  if (icm20948.accelDataIsReady())
  {
    icm20948.readAccelData(&accel_x, &accel_y, &accel_z);
    sprintf(sensor_string_buff, "Accel (g): [%f,%f,%f]", accel_x, accel_y, accel_z);
    Serial.print(sensor_string_buff);
    Serial.print("  ||  ");
  }
  Serial.println();
}
