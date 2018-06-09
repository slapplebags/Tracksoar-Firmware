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

// Trackuino custom libs
#include "config.hpp"
#include "afsk_avr.hpp"
#include "aprs.hpp"
#include "gps.hpp"
#include "power.hpp"
#include "sensors_avr.hpp"

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
	#include <Arduino.h>
#else
	#error "Please use a arduweenie version from this decade."
#endif

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Though not used here, we need to include Wire.h in this file for other code:
#include <Wire.h>
// Same is true for SPI.h
#include <SPI.h>
#include <avr/wdt.h>

// Module variables
static int32_t next_aprs = 0;


void setup()
{
	LED_DDR  |= _BV(LED_PIN_BIT);

	DEBUG_UART.begin(115200);
	GPS_UART.begin(GPS_BAUDRATE);

	#ifdef DEBUG_RESET
		DEBUG_UART.println("RESET");
	#endif

	afsk_setup();
	gps_setup();
	sensors_setup();


	// Do not start until we get a valid time reference
	// for slotted transmissions.
	if (APRS_SLOT >= 0)
	{
		do
		{
			while (! GPS_UART.available())
			{
				power_save();
				DEBUG_UART.println("Looping for gps lock.");
			}
		}
		while (! gps_decode(GPS_UART.read()));

		next_aprs = millis() + 1000 * (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
	}
	else
	{
		next_aprs = millis();
	}


	watchdogSetup();

	// TODO: beep while we get a fix, maybe indicating the number of
	// visible satellites by a series of short beeps?
}

void get_pos()
{
	// Get a valid position from the GPS
	int valid_pos = 0;
	uint32_t timeout = millis();

	do
	{
		if (GPS_UART.available())
		{
			valid_pos = gps_decode(GPS_UART.read());
		}
	}
	while ( ((millis() - timeout) < VALID_POS_TIMEOUT) && ! valid_pos) ;

	#if defined(DEBUG_GPS)
		if (valid_pos)
			DEBUG_UART.println("Have valid GPS position fix.");
		else
			DEBUG_UART.println("Failed to receive valid GPS position fix.");
	#endif
}

void loop()
{


	// Time for another APRS frame
	safe_pet_watchdog();

	if ((int32_t) (millis() - next_aprs) >= 0)
	{
		DEBUG_UART.println("Doing transmit");

		#ifdef DEBUG_SENS
			DEBUG_UART.print("Temp=");
			DEBUG_UART.print(sensors_temperature());
			DEBUG_UART.print(", pressure=");
			DEBUG_UART.print(sensors_pressure());
			DEBUG_UART.print(", humidity=");
			DEBUG_UART.print(sensors_humidity());
			DEBUG_UART.print(", battery=");
			DEBUG_UART.print(sensors_battery());
			DEBUG_UART.println(".");
		#endif

		get_pos();
		aprs_send();
		safe_pet_watchdog();
		next_aprs += APRS_PERIOD * 1000L;

		while (afsk_flush())
		{
			power_save();
			safe_pet_watchdog();
		}

		#ifdef DEBUG_MODEM
			// Show modem ISR stats from the previous transmission
			afsk_debug();
			DEBUG_UART.println("Loop!");
		#endif
		DEBUG_UART.println("Message sent. Sleeping");
	}

	power_save(); // Incoming GPS data or interrupts will wake us up
}
