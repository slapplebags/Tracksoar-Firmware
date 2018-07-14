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

#include "config.hpp"
#include "ax25.hpp"
#include "gps.hpp"
#include "aprs.hpp"
#include "sensors_avr.hpp"
#include <stdio.h>
#include <stdlib.h>
#if (ARDUINO + 1) >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

// Module functions
float meters_to_feet(float m)
{
	// 10000 ft = 3048 m
	return m / 0.3048;
}

long max_alt = 0;

// Exported functions
void aprs_send()
{

	char temp[12];                   // Temperature (int/ext)
	const struct s_address addresses[] =
	{
		{D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
		{S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)

		#ifdef DIGI_PATH1
			{DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
		#endif
		#ifdef DIGI_PATH2
			{DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
		#endif
	};

	int address_chain_sz = 2;

	#ifdef DIGI_PATH1
		address_chain_sz += 1;
	#endif
	#ifdef DIGI_PATH2
		address_chain_sz += 1;
	#endif

	// If the current altitude is above the digipeating threshold,
	// no longer send the digipeat segments of the path.
	if (gps_altitude > DISABLE_PATH_ABOVE_ALT_METRES)
		address_chain_sz = 2;

	// Track the current altitude, and the max altitude.
	long cur_alt = (long)(meters_to_feet(gps_altitude) + 0.5);

	if (cur_alt > max_alt)
		max_alt = cur_alt;

	ax25_send_header(addresses, address_chain_sz);
	ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
	ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
	ax25_send_byte('h');
	ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
	ax25_send_byte('/');                // Symbol table
	ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
	ax25_send_byte('O');                // Symbol: O=balloon, -=QTH
	snprintf(temp, 4, "%03d", (int)(gps_course + 0.5));
	ax25_send_string(temp);             // Course (degrees)
	ax25_send_byte('/');                // and
	snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
	ax25_send_string(temp);             // speed (knots)
	ax25_send_string("/A=");            // Altitude (feet). Goes anywhere in the comment area
	snprintf(temp, 7, "%06ld", cur_alt);
	ax25_send_string(temp);
	ax25_send_string("/MaxA=");            // Altitude (feet). Goes anywhere in the comment area
	snprintf(temp, 7, "%06ld", max_alt);
	ax25_send_string(temp);
	// Pressure: "/Pa=12345"
	ax25_send_string("/Pa=");
	snprintf(temp, 6, "%ld", sensors_pressure());
	ax25_send_string(temp);
	// Humidity: "/Rh=84.56"
	ax25_send_string("/Rh=");
	dtostrf(sensors_humidity(), -1, 2, temp);
	ax25_send_string(temp);
	// Temperature
	// "Ti=-8.70"
	ax25_send_string("/Ti=");
	dtostrf(sensors_temperature(), -1, 2, temp);
	ax25_send_string(temp);
	ax25_send_string("/batt=");
	dtostrf(sensors_battery(), -1, 2, temp);
	ax25_send_string(temp);

	ax25_send_byte(' ');
	ax25_send_string(APRS_COMMENT);     // Comment
	ax25_send_footer();

	ax25_flush_frame();                 // Tell the modem to go
}
