#include "wire_extension.h"
#include <SD.h>
#include "LSM303.h"
#include "BMP180.h"
LSM303 lsm303= LSM303();
    void begin(void);
  char startMagnetometer(void);
  char startAccelerometer(void);
  char getMagnetometer(S3DVEC &magnet);
  char getAccelerometer(S3DVEC &accel);
  char setMagnetometerGain(lsm303MagGain gain);
S3DVEC mag;
S3DVEC acc;

void print3d(S3DVEC data){
  Serial.print("X = ");
  Serial.print(data.x);
  Serial.print(" Y = ");
  Serial.print(data.y);
  Serial.print(" Z = ");
  Serial.print(data.z);
}

void setup() {
  Serial.begin(9600);
  lsm303.begin();
}

void loop() {
  lsm303.startMagnetometer();
  delay(1000);
  lsm303.getMagnetometer(mag);
  Serial.print("MAG = ");
  print3d(mag);
  Serial.println(" GAUSS");
  lsm303.startAccelerometer();
  delay(1000);
  lsm303.getAccelerometer(mag);
  Serial.print("ACC = ");
  print3d(mag);
  Serial.println(" m/s**2");

}
