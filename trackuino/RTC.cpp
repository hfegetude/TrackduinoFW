#include "RTC.h"

//  ____ _____ ____
// |  _ \_   _/ ___|
// | |_) || || |
// |  _ < | || |___
// |_| \_\|_| \____|
//

void setDateRTC( TIME_FORMAT current)
{
        Wire.beginTransmission(DS1307_I2C_ADDRESS);
        Wire.write(0);
        Wire.write(decToBcd(current.second));
        Wire.write(decToBcd(current.minute));
        Wire.write(decToBcd(current.hour));
        Wire.write(decToBcd(current.dayOfWeek));
        Wire.write(decToBcd(current.dayOfMonth));
        Wire.write(decToBcd(current.month));
        Wire.write(decToBcd(current.year));
        Wire.endTransmission();

}

// Gets the date and time from the ds1307
TIME_FORMAT getDateRTC()
{
        TIME_FORMAT current;
        Wire.beginTransmission(DS1307_I2C_ADDRESS);
        // Reset the register pointer
        //Reading from bit 0
// http://www.8051projects.net/wiki/DS1307_I2C_RTC_Interfacing_Tutorial
        Wire.write(0);
        Wire.endTransmission();
        Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

        // A few of these need masks because certain bits are control bits
        current.second     = bcdToDec(Wire.read() & 0x7f);
        current.minute     = bcdToDec(Wire.read());
        current.hour       = bcdToDec(Wire.read() & 0x3f);// Need to change this if 12 hour am/pm
        current.dayOfWeek  = bcdToDec(Wire.read());
        current.dayOfMonth = bcdToDec(Wire.read());
        current.month      = bcdToDec(Wire.read());
        current.year       = bcdToDec(Wire.read());
        return current;
}
