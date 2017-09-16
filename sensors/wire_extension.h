#ifndef WIRE_EXTENSION_H
#define WIRE_EXTENSION_H

#include <Wire.h>

class WireExtension: public TwoWire {
public:
  	char readInt(char address, char reg,int16_t &value);
    char readUInt(char address, char reg, uint16_t &value);
    char readBytes(char address,unsigned char *values, char length);
    char writeBytes(char address, unsigned char *values, char length);
    char getError(void );
private:
    uint8_t _error;
};




#endif
