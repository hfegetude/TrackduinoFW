#ifndef BMP180_H
#define BMP180_H

#define BMP_I2C_ADRESS 0x77
#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_RESULT 0xF6
#define BMP180_COMMAND_TEMPERATURE 0x2E
#define BMP180_COMMAND_PRESSURE0 0x34
#define BMP180_COMMAND_PRESSURE1 0x74
#define BMP180_COMMAND_PRESSURE2 0xB4
#define BMP180_COMMAND_PRESSURE3 0xF4

#include "wire_extension.h"

class BMP180: public WireExtension {

public:
    BMP180();
    char begin();
    char startTemperature(void);
    char getTemperature(double &T);
    char getPressure(double &P, double &T);
    char startPressure(char oversampling);



private:
    int16_t AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
		uint16_t AC4,AC5,AC6;
    double c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;
};




#endif
