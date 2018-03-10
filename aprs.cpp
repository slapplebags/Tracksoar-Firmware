/* trackuino copyright (C) 2010  EA5HAV Javi

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "sensors_avr.h"
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

// Module variables

int32_t alt_ft;
int32_t prev_alt_ft = 0;
int32_t last_alt_ft = 0;
int32_t peak_alt_ft = 0;
bool burst = 0;
float feet_minute = 0;
int ascent_rate = 0;

#ifdef DEBUG_MAX_ALTITUDE
int32_t min_alt_ft = 8000;
int16_t dif = 0;
int16_t big_dif_P = 0;
int16_t big_dif_N = 0;
#endif

// Module functions

float meters_to_feet(float m)
{ return m / 0.3048;    // 10000 ft = 3048 m
}

#ifdef DEBUG_MAX_ALTITUDE
void print_debug_max_altitude()
{ Serial.println();
  Serial.println(F("DEBUG_MAX_ALTITUDE - aprs.cpp"));
  Serial.print('[');
  Serial.print(millis());
  Serial.print(F("] gps_time: "));
  Serial.print(gps_time);
  Serial.print(F("  PrevAlt: "));
  Serial.print(prev_alt_ft);
  Serial.print(F("  Alt: "));
  Serial.print(alt_ft);
  dif = alt_ft - prev_alt_ft;
  Serial.print(F("  Dif: "));
  Serial.print(dif);
  if (dif > big_dif_P) big_dif_P = dif;
  if (dif < big_dif_N) big_dif_N = dif;
  Serial.print(F("  BigPosDif: "));
  Serial.print(big_dif_P);
  Serial.print(F("  BigNegDif: "));
  Serial.print(big_dif_N);
  Serial.print(F(" -- BURST: "));
  Serial.print(burst);
  Serial.print(F(" -- PEAK: "));
  Serial.print(peak_alt_ft);
  Serial.print(F("  MIN: "));
  Serial.print(min_alt_ft);
  Serial.print(F("  DIF: "));
  Serial.print(peak_alt_ft - min_alt_ft);
}
#endif

// Exported functions

void check_altitude()         // Check Altitude for Peak and Burst
{ alt_ft = (meters_to_feet(gps_altitude) + 0.5);
  if (last_alt_ft == 0) last_alt_ft = alt_ft;   // Initialize last_alt to current altitude
  if (prev_alt_ft == 0) prev_alt_ft = alt_ft;   // Initialize prev_alt to current altitude
  if (!burst && peak_alt_ft - alt_ft > 1000)    // after burst payload will fall about 1600ft in 10 sec's and
    burst = 1;          // checking for a 1000ft loss should be enough to filter out any inaccurate GPS data.
  if (alt_ft > peak_alt_ft)
    peak_alt_ft = alt_ft;
#ifdef DEBUG_MAX_ALTITUDE
  if (alt_ft < min_alt_ft)  min_alt_ft = alt_ft;
#endif
}

void finish_checking_altitude()
{
#ifdef DEBUG_MAX_ALTITUDE
  print_debug_max_altitude();
  Serial.println();
#endif
  prev_alt_ft = alt_ft;
}

void aprs_send()
{ // Calculate Ascent Rate (ft/min)
  feet_minute = (alt_ft - last_alt_ft) * (60.0 / (float)APRS_PERIOD);
  // Round off ft/minute and make ascent_rate an interger
  if (feet_minute > 0)
    ascent_rate = feet_minute + 0.5;
  if (feet_minute < 0)
    ascent_rate = feet_minute - 0.5;

#ifdef DEBUG_MAX_ALTITUDE
  print_debug_max_altitude();
  Serial.print(F(" - aprs.cpp 119-126 LastAlt="));
  Serial.print(last_alt_ft);
  Serial.print(F("  Dif="));
  Serial.print(alt_ft - last_alt_ft);
  Serial.print(F("  Ft/M="));
  Serial.print(feet_minute);
  Serial.print(F("  AR="));
  Serial.print(ascent_rate);
#endif

  char temp[12];
  const struct s_address addresses[] =
  {
    {D_CALLSIGN, D_CALLSIGN_ID},      // Destination callsign ( = APRS, 0 from config.h )
    {S_CALLSIGN, S_CALLSIGN_ID},      // Source callsign      ( from config.h )
    {DIGI_PATH1, DIGI_PATH1_TTL},     // DIGI_PATH1           ( = WIDE2, 1 from config.h )
  };
  ax25_send_header(addresses, sizeof(addresses) / sizeof(s_address));
  ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
  ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                // Symbol table
  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte('O');                // Symbol: O = balloon
  snprintf(temp, 4, "%03d", (int)(gps_course + 0.5));
  ax25_send_string(temp);             // Course (degrees)
  ax25_send_byte('/');                //
  snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
  ax25_send_string(temp);             // speed (knots)
  ax25_send_string("/A=");
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
  ax25_send_string(temp);             // Altitude (feet)
#ifdef SEND_SATS                      // # of satellites in view
  ax25_send_string("/Sats");
  snprintf(temp, 3, "%d", gps_num_sats);
  ax25_send_string(temp);
#endif
#ifdef SEND_HDOP                      // GPS HDOP
  ax25_send_string("/HDOP");
  dtostrf(gps_hdop, -4, 2, temp);
  ax25_send_string(temp);
#endif
#ifdef SEND_PA
  ax25_send_string("/Pa");
  snprintf(temp, 6, "%ld", sensors_pressure());
  ax25_send_string(temp);             // Pressure (Pa)
#endif
#ifdef SEND_RH
  ax25_send_string("/Rh");
  dtostrf(sensors_humidity(), -1, 2, temp);
  ax25_send_string(temp);             // Humidity
#endif
#ifdef SEND_TI
  ax25_send_string("/Ti");           // Temperature Internal /Ti=
  dtostrf(sensors_temperature(), -1, 1, temp);
  ax25_send_string(temp);             // Temperature Internal (deg.C)
#endif
#ifdef SEND_AR                        // Ascent Rate
  ax25_send_string("/AR");
  snprintf(temp, 6, "%d", ascent_rate);
  ax25_send_string(temp);
#endif

  // Code left for when an external Tracksoar temperature sensor is added - **************************************
  // ax25_send_string(" / Te = ");
  // snprintf(temp, 6, " % d", sensors_ext_lm60());
  // ax25_send_string(temp);          // Temperature External (deg.C)

  // Code left if Tracksoar PCB is ever modified to allow reporting Battery Voltage - ****************************
  // ax25_send_string(" / V = ");
  // snprintf(temp, 6, " % d", sensors_vin());
  // ax25_send_string(temp);          // Battery Voltage

#ifdef SEND_MAX_ALT                   // include Burst Altitude in comment field if burst detected
  if (burst)
  { ax25_send_string(" B@");
    snprintf(temp, 7, "%06ld", peak_alt_ft);
    ax25_send_string(temp);
  }
#endif

  ax25_send_byte(' ');
  ax25_send_string(APRS_COMMENT);
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go

  prev_alt_ft = alt_ft; // Altitude at the time it was last checked for Peak and Burst (each 10 sec's)
  last_alt_ft = alt_ft; // Altitude at the time of the last transmission. Used to calculate ascent rate.
}
