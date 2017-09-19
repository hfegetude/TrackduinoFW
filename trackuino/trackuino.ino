/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

 /*
  * Version 0.2
  * 
  * Upgrade:
  *   Fixed I2C communication and aprs frame creation
  *   Fixed compilation warnings
  * 
  */

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif


// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include  <Wire.h>
#include "wire_extension.h"
#include "BMP180.h"
#include "LSM303.h"
#include "MPU6050.h"
#include "RTC.h"


/*Initialize all sensors */
BMP180 bmp180 = BMP180();
LSM303 lsm303 = LSM303();
MPU6050 mpu6050 = MPU6050();



// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;


void setup()
{
  Serial3.begin(GPS_BAUDRATE);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif
  afsk_setup();
  gps_setup();
  Wire.begin();
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  //Begin all sensors
  bmp180.begin();
  lsm303.begin();
  lsm303.setMagnetometerGain(LSM303_MAGGAIN_4_7);
  mpu6050.begin();
  mpu6050.setRange(0);
  mpu6050.startGyro();

  TIME_FORMAT launch = {
    0, //seconds
    0, //minutes
    0, //hours
    0, //day of week
    16, //day of month
    3, //month
    95 //year
  };
  setDateRTC(launch);
  // Do not start until we get a valid time reference
  // for slotted transmissions.
  if ( (APRS_SLOT >= 0) & 0) {
    do {
      while (! Serial3.available())
        power_save();
    } while (! gps_decode(Serial3.read()));

    next_aprs = millis() + 1000 *
      (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else {
    next_aprs = millis();
  }
  // TODO: beep while we get a fix, maybe indicating the number of
  // visible satellites by a series of short beeps?
}

void get_pos()
{
  // Get a valid position from the GPS
  int valid_pos = 0;
  uint32_t timeout = millis();

#ifdef DEBUG_GPS
  Serial.println("\nget_pos()");
#endif

  gps_reset_parser();

  do {
    if (Serial3.available())
      valid_pos = gps_decode(Serial3.read());
  } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;
}

void loop()
{ 
  
  // Time for another APRS frame
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    aprs_send(bmp180,lsm303, mpu6050 );
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      power_save();
    }

#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif

  } else {
    // Discard GPS data received during sleep window
    while (Serial3.available()) {
      Serial3.read();
    }
  }


  power_save(); // Incoming GPS data or interrupts will wake us up

}
