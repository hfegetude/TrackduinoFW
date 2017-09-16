#include "wire_extension.h"
#include "LSM303.h"

LSM303::LSM303(){}
void LSM303::begin(){
  if (TWCR == 0) // do this check so that Wire only gets initialized once
  {
   WireExtension::begin();
  }
}

char LSM303::startAccelerometer(){
  char data[2];
  char error;
  data[0] = LSM303_REGISTER_ACCEL_CTRL_REG1_A;
  data[1] = LSM303_ACCELRATE_LP_1_0;
  error = writeBytes(LSM303_I2C_ADDRES_ACCEL, data, 2);
  return(error);
}

char LSM303::getAccelerometer(S3DVEC &accel){
  char data[6];
  char ok;
  data[0] = LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80;
  ok = readBytes(LSM303_I2C_ADDRES_ACCEL, data, 6);
	accel.x = (double)(((data[1] << 8) | data[0]) >> 4)*  _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
  accel.y = (double)(((data[3] << 8) | data[2]) >> 4)*  _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
  accel.z = (double)(((data[5] << 8) | data[4]) >> 4)*  _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
  return(ok);
}

char LSM303::startMagnetometer(){
  char data[2];
  data[0] = LSM303_REGISTER_MAG_MR_REG_M;
  data[1] = 0x01;
  return( writeBytes(LSM303_I2C_ADDRES_MAGNE, data, 2));
}

char LSM303::getMagnetometer(S3DVEC &magn){
  char data[6];
  char ok;
  data[0] = LSM303_REGISTER_MAG_OUT_X_H_M | 0x80;
  ok = readBytes(LSM303_I2C_ADDRES_MAGNE, data, 6);
  magn.x = (double)((data[0] << 8) | data[1]) / _lsm303Mag_Gauss_LSB_XY;
  magn.y = (double)((data[2] << 8) | data[3]) / _lsm303Mag_Gauss_LSB_XY;
  magn.z = (double)((data[4] << 8) | data[5] ) / _lsm303Mag_Gauss_LSB_Z;
  return(ok);
}

char LSM303::setMagnetometerGain(lsm303MagGain gain){
  char data[2];
  data[0] = LSM303_REGISTER_MAG_CRB_REG_M;
  data[1] = gain;
  switch(gain)
  {
    case LSM303_MAGGAIN_1_3:
      _lsm303Mag_Gauss_LSB_XY = 1100;
      _lsm303Mag_Gauss_LSB_Z  = 980;
      break;
    case LSM303_MAGGAIN_1_9:
      _lsm303Mag_Gauss_LSB_XY = 855;
      _lsm303Mag_Gauss_LSB_Z  = 760;
      break;
    case LSM303_MAGGAIN_2_5:
      _lsm303Mag_Gauss_LSB_XY = 670;
      _lsm303Mag_Gauss_LSB_Z  = 600;
      break;
    case LSM303_MAGGAIN_4_0:
      _lsm303Mag_Gauss_LSB_XY = 450;
      _lsm303Mag_Gauss_LSB_Z  = 400;
      break;
    case LSM303_MAGGAIN_4_7:
      _lsm303Mag_Gauss_LSB_XY = 400;
      _lsm303Mag_Gauss_LSB_Z  = 355;
      break;
    case LSM303_MAGGAIN_5_6:
      _lsm303Mag_Gauss_LSB_XY = 330;
      _lsm303Mag_Gauss_LSB_Z  = 295;
      break;
    case LSM303_MAGGAIN_8_1:
      _lsm303Mag_Gauss_LSB_XY = 230;
      _lsm303Mag_Gauss_LSB_Z  = 205;
      break;
  }
  return( writeBytes(LSM303_I2C_ADDRES_MAGNE, data, 2));
  
}

