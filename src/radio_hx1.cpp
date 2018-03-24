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
#include "radio_hx1.hpp"
#if (ARDUINO + 1) >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif


void RadioHx1::setup()
{
	// Configure pins
	// Radio enable pin
	TX_ENABLE_PORT &= ~_BV(TX_ENABLE_PIN_BIT);
	TX_ENABLE_DDR  |=  _BV(TX_ENABLE_PIN_BIT);

	// Tx out is driven by timer 4.
	TX_OUT_DDR     |=  _BV(TX_OUT_PIN_BIT);

}

void RadioHx1::ptt_on()
{
	DEBUG_UART.println("Enabling transmitter");
	TX_ENABLE_PORT |= _BV(TX_ENABLE_PIN_BIT);
	delay(25);   // The HX1 takes 5 ms from PTT to full RF, give it 25
}

void RadioHx1::ptt_off()
{
	DEBUG_UART.println("Disabling transmitter");
	TX_ENABLE_PORT &= ~_BV(TX_ENABLE_PIN_BIT);
}
