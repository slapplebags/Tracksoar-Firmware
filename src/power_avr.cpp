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
#ifdef AVR

#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#if (ARDUINO + 1) >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#include "config.hpp"
#include "power.hpp"
#include <avr/wdt.h>

// void disable_bod_and_sleep()
// {
// 	/* This will turn off brown-out detection while
// 	 * sleeping. Unfortunately this won't work in IDLE mode.
// 	 * Relevant info about BOD disabling: datasheet p.44
// 	 *
// 	 * Procedure to disable the BOD:
// 	 *
// 	 * 1. BODSE and BODS must be set to 1
// 	 * 2. Turn BODSE to 0
// 	 * 3. BODS will automatically turn 0 after 4 cycles
// 	 *
// 	 * The catch is that we *must* go to sleep between 2
// 	 * and 3, ie. just before BODS turns 0.
// 	 */
// 	unsigned char mcucr;

// 	cli();
// 	mcucr = MCUCR | (_BV(BODS) | _BV(BODSE));
// 	MCUCR = mcucr;
// 	MCUCR = mcucr & (~_BV(BODSE));
// 	sei();
// 	sleep_mode();    // Go to sleep
// }



void safe_pet_watchdog(void)
{
	if (UDADDR & _BV(ADDEN) || true)
		return;
	wdt_reset();
}

void watchdogSetup(void)
{
	// If we have a active USB connection, don't turn on the WDT, because it breaks the
	// bootloader.
	if (UDADDR & _BV(ADDEN) || true)
		return;

	cli();
	wdt_reset();
	/*
	 WDTCSR configuration:
	 WDIE = 1: Interrupt Enable
	 WDE = 1 :Reset Enable
	 See table for time-out variations:
	 WDP3 = 0 :For 1000ms Time-out
	 WDP2 = 1 :For 1000ms Time-out
	 WDP1 = 1 :For 1000ms Time-out
	 WDP0 = 0 :For 1000ms Time-out
	*/
	// Enter Watchdog Configuration mode:
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	// Set Watchdog settings:
	WDTCSR = (0 << WDIE) | (1 << WDE) |
	         (1 << WDP3) | (0 << WDP2) | (0 << WDP1) |
	         (1 << WDP0);
	DEBUG_UART.println("WDT reset");
	sei();
}



void power_save(void)
{

	/* Enter power saving mode. SLEEP_MODE_IDLE is the least saving
	 * mode, but it's the only one that will keep the UART running.
	 * In addition, we need timer0 to keep track of time, timer 1
	 * to drive the buzzer and timer2/4 to keep pwm output at its rest
	 * voltage.
	 */

	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	power_adc_disable();
	power_spi_disable();
	power_twi_disable();

	safe_pet_watchdog();

	LED_PORT &= ~_BV(LED_PIN_BIT);
	sleep_mode();    // Go to sleep
	LED_PORT |= _BV(LED_PIN_BIT);

	sleep_disable();  // Resume after wake up
	power_all_enable();
}


#endif // #ifdef AVR
