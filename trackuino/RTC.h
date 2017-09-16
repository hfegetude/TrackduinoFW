#ifndef RTC_H
#define RTC_H
#define DS1307_I2C_ADDRESS 0x68
#include <Wire.h>

typedef struct TIME_FORMAT_t {
        uint8_t second;  // 0-59
        uint8_t minute;  // 0-59
        uint8_t hour;                       // 1-23
        uint8_t dayOfWeek;                  // 1-7
        uint8_t dayOfMonth;                 // 1-28/29/30/31
        uint8_t month;                      // 1-12
        uint8_t year;
} TIME_FORMAT;



//  ____ _____ ____
// |  _ \_   _/ ___|
// | |_) || || |
// |  _ < | || |___
// |_| \_\|_| \____|
//

//Currently not working, Ijust get garbage on the board (code works, it's the electrical connection)
void setDateRTC(TIME_FORMAT current);
TIME_FORMAT getDateRTC();

inline uint8_t decToBcd(uint8_t val)
{
       return ( (val/10*16) + (val%10) );
}

inline uint8_t bcdToDec(uint8_t val)
{
       return ( (val/16*10) + (val%16) );
}
#endif
