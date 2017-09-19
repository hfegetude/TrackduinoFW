#include "wire_extension.h"
#include "MPU6050.h"

MPU6050::MPU6050(){};
void MPU6050::begin(){
  if (TWCR == 0) // do this check so that Wire only gets initialized once
  {
   WireExtension::begin();
  }
}

char MPU6050::startGyro(){
  uint8_t data[4];
  data[0] = MPU6050_RA_PWR_MGMT_1;
  data[1] = MPU6050_PWR1_CYCLE_BIT;
  data[2] = MPU6050_RA_PWR_MGMT_2;
  data[3] = 0x00; //set freq to 1.25Hz
  writeBytes(MPU6050_DEFAULT_ADDRESS, data, 4);
}

char MPU6050::setRange(uint8_t fs){
  uint8_t data[2];
  data[0] = MPU6050_RA_GYRO_CONFIG;
  switch (fs) {
    case 0:
    data[1] = MPU6050_GYRO_FS_250;
    _scale = (double) 131.0;
    break;
    case 1:
    data[1] = MPU6050_GYRO_FS_500;
    _scale = (double) 65.5;
    break;
    case 2:
    data[1] = MPU6050_GYRO_FS_1000;
    _scale = (double) 32.8;
    break;
    case 3:
    data[1] = MPU6050_GYRO_FS_2000;
    _scale = (double) 16.4;
    break;
  }
  return(writeBytes(MPU6050_DEFAULT_ADDRESS, data, 2));

}

char MPU6050::getGyro(S3DVEC &gyro){
  char data[6];
  char ok;
  data[0] = MPU6050_RA_GYRO_XOUT_H;
  ok = readBytes(MPU6050_DEFAULT_ADDRESS, data, 6);
	gyro.x = (double)(((data[0] << 8) | data[1]))/  _scale;
  gyro.y = (double)(((data[2] << 8) | data[3]))/  _scale;
  gyro.z = (double)(((data[4] << 8) | data[5]))/  _scale;
  return(ok);
}
