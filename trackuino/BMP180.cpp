#include "wire_extension.h"
#include "BMP180.h"
#include <math.h>

BMP180::BMP180(){
}

char BMP180::begin(){
  if (TWCR == 0) // do this check so that Wire only gets initialized once
  {
   WireExtension::begin();
  }
  double c3,c4,b1;
  if (readInt(BMP_I2C_ADRESS,0xAA,AC1) &&
		readInt(BMP_I2C_ADRESS,0xAC,AC2) &&
		readInt(BMP_I2C_ADRESS,0xAE,AC3) &&
    readUInt(BMP_I2C_ADRESS,0xB0,AC4) &&
		readUInt(BMP_I2C_ADRESS,0xB2,AC5) &&
		readUInt(BMP_I2C_ADRESS,0xB4,AC6) &&
		readInt(BMP_I2C_ADRESS,0xB6,VB1) &&
		readInt(BMP_I2C_ADRESS,0xB8,VB2) &&
		readInt(BMP_I2C_ADRESS,0xBA,MB) &&
		readInt(BMP_I2C_ADRESS,0xBC,MC) &&
		readInt(BMP_I2C_ADRESS,0xBE,MD))
	{
		c3 = 160.0 * pow(2,-15) * AC3;
		c4 = pow(10,-3) * pow(2,-15) * AC4;
		b1 = pow(160,2) * pow(2,-30) * VB1;
		c5 = (pow(2,-15) / 160) * AC5;
		c6 = AC6;
		mc = (pow(2,11) / pow(160,2)) * MC;
		md = MD / 160.0;
		x0 = AC1;
		x1 = 160.0 * pow(2,-13) * AC2;
		x2 = pow(160,2) * pow(2,-25) * VB2;
		y0 = c4 * pow(2,15);
		y1 = c4 * c3;
		y2 = c4 * b1;
		p0 = (3791.0 - 8.0) / 1600.0;
		p1 = 1.0 - 7357.0 * pow(2,-20);
		p2 = 3038.0 * 100.0 * pow(2,-36);
		return(1);
	}
	else
	{
		return(0);
  }
}


char BMP180::startTemperature(void)
// Begin a temperature reading.
// Will return delay in ms to wait, or 0 if I2C error
{
	unsigned char data[2], result;

	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;
	result = writeBytes(BMP_I2C_ADRESS,data, 2);
	if (result) // good write?
		return(5); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}


char BMP180::getTemperature(double &T)
{
	unsigned char data[2];
	char result;
	double tu, a;

	data[0] = BMP180_REG_RESULT;

	result = readBytes(BMP_I2C_ADRESS, data, 2);
	if (result) // good read, calculate temperature
	{
		tu = (data[0] * 256.0) + data[1];
		a = c5 * (tu - c6);
		T = a + (mc / (a + md));

	}
	return(result);
}



char BMP180::getPressure(double &P, double &T)
{
  unsigned char data[3];
  char result;
  double pu,s,x,y,z;
  
  data[0] = BMP180_REG_RESULT;

  result = readBytes(BMP_I2C_ADRESS ,data, 3);
  if (result) // good read, calculate pressure
  {
    pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);
    
    s = T - 25.0;
    x = (x2 * pow(s,2)) + (x1 * s) + x0;
    y = (y2 * pow(s,2)) + (y1 * s) + y0;
    z = (pu - x) / y;
    P = (p2 * pow(z,2)) + (p1 * z) + p0;

  }
  return(result);
}

char BMP180::startPressure(char oversampling)
// Begin a pressure reading.
// Oversampling: 0 to 3, higher numbers are slower, higher-res outputs.
// Will return delay in ms to wait, or 0 if I2C error.
{
  unsigned char data[2], result, delay;
  
  data[0] = BMP180_REG_CONTROL;

  switch (oversampling)
  {
    case 0:
      data[1] = BMP180_COMMAND_PRESSURE0;
      delay = 5;
    break;
    case 1:
      data[1] = BMP180_COMMAND_PRESSURE1;
      delay = 8;
    break;
    case 2:
      data[1] = BMP180_COMMAND_PRESSURE2;
      delay = 14;
    break;
    case 3:
      data[1] = BMP180_COMMAND_PRESSURE3;
      delay = 26;
    break;
    default:
      data[1] = BMP180_COMMAND_PRESSURE0;
      delay = 5;
    break;
  }
  result = writeBytes(BMP_I2C_ADRESS, data, 2);
  if (result) // good write?
    return(delay); // return the delay in ms (rounded up) to wait before retrieving data
  else
    return(0); // or return 0 if there was a problem communicating with the BMP
}
