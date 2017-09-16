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

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "wire_extension.h"
#include "BMP180.h"
#include "LSM303.h"
#include "MPU6050.h"
#include "RTC.h"
#include "ov7670.h"
#include "ov7670fifo.h"
#include <stdio.h>
#include <stdlib.h>
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

extern BMP180 bmp180;
extern LSM303 lsm303 ;
extern MPU6050 mpu6050 ;

// Exported functions
void aprs_send()
{
  char temp[12];                   // Temperature (int/ext)
  S3DVEC temp_3D;
  TIME_FORMAT current_time = getDateRTC();
  double temperature;
  double pressure;
  const struct s_address addresses[] = {
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
  };
  setupOV7670();
  captureImgOV7670(160*2,120);

  setupOV7670fifo();
  transmit_photo(640, 480);
  capture_frame();

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('/');
  snprintf(temp, 7 ,"%02d%02d%02dz" ,current_time.dayOfMonth, current_time.month, current_time.year );
  ax25_send_byte(temp);              // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  snprintf(temp, 7 ,"%02d%02d%02dz" ,current_time.hour, current_time.minute, current_time.second );
  ax25_send_byte(temp);



  ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                // Symbol table
  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte('O');                // Symbol: O=balloon, -=QTH


  mpu6050.startGyro();
  mpu6050.getGyro(temp_3D);
  //send rotation
  ax25_send_byte('/RoX=');                // and
  snprintf(temp, 12 , "%.2lf", temp_3D.x  );
  ax25_send_byte(temp);

  ax25_send_byte('/RoY=');                // and
  snprintf(temp, 12 , "%.2lf", temp_3D.y  );
  ax25_send_byte(temp);

  ax25_send_byte('/RoZ=');                // and
  snprintf(temp, 12 , "%.2lf", temp_3D.z  );
  ax25_send_byte(temp);


  lsm303.startAccelerometer();
  lsm303.getAccelerometer(temp_3D);
  //send acceleration
  ax25_send_byte('/AcX=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.x  );
  ax25_send_byte(temp);

  ax25_send_byte('/AcY=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.y  );
  ax25_send_byte(temp);

  ax25_send_byte('/AcZ=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.z  );
  ax25_send_byte(temp);

  lsm303.startMagnetometer();
  lsm303.getMagnetometer(temp_3D);
  //send acceleration
  ax25_send_byte('/MaX=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.x  );
  ax25_send_byte(temp);

  ax25_send_byte('/MaY=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.y  );
  ax25_send_byte(temp);

  ax25_send_byte('/MaZ=');                // and
  snprintf(temp, 12, "%.2lf", temp_3D.z  );
  ax25_send_byte(temp);

  //send temperature
  delay(bmp180.startTemperature());
  bmp180.getTemperature(temperature);
  ax25_send_byte('/T=');
  snprintf(temp, 12, "%.2lf", temperature);
  ax25_send_byte(temp);

  delay( bmp180.startPressure(4) );
  bmp180.getPressure(pressure, temperature);
  ax25_send_byte('/P=');
  snprintf(temp, 12, "%.2lf", temperature);
  ax25_send_byte(temp);
  //ax25_send_string(APRS_COMMENT);     // Comment

  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
}
