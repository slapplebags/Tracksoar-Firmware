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
#include "afsk_pic32.h"
#include "aprs.h"
#include "nyanpollo.h"
#include "buzzer.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"
#include <avr/wdt.h>

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Though not used here, we need to include Wire.h in this file for other code:
#include <Wire.h>
// Same is true for SPI.h
#include <SPI.h>

// Module variables
static int32_t next_aprs = 0;


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);
  watchdogSetup();
// deactivate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    cbi(PORTC, 4);
    cbi(PORTC, 5);

  Serial.begin(GPS_BAUDRATE);
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif

  buzzer_setup();
  afsk_setup();
  gps_setup();
  sensors_setup();

#ifdef DEBUG_SENS
  Serial.print("Ti=");
  Serial.print(sensors_int_lm60());
  Serial.print(", Te=");
  Serial.print(sensors_ext_lm60());
  Serial.print(", Vin=");
  Serial.println(sensors_vin());
#endif

  // Do not start until we get a valid time reference
  // for slotted transmissions.
  if (APRS_SLOT >= 0) {
    do {
      while (! Serial.available())
        power_save();
    } while (! gps_decode(Serial.read()));

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
  do {
    if (Serial.available())
      valid_pos = gps_decode(Serial.read());
  } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;

  if (valid_pos) {
    if (gps_altitude > BUZZER_ALTITUDE) {
      buzzer_off();   // In space, no one can hear you buzz
    } else {
      buzzer_on();
    }
  }
}

void loop()
{
  // Time for another APRS frame
  wdt_reset();
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();
    aprs_send();
    wdt_reset();
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      power_save();
      wdt_reset();
    }

#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif
  }

  // We send this every time we get an update, not only when APRS needs to fire.
  nyanpollo_send();

  power_save(); // Incoming GPS data or interrupts will wake us up
}

void watchdogSetup(void)
{
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
WDTCSR |= (1<<WDCE) | (1<<WDE);
// Set Watchdog settings:
 WDTCSR = (0<<WDIE) | (1<<WDE) |
(1<<WDP3) | (0<<WDP2) | (0<<WDP1) |
(1<<WDP0);
Serial.println("WDT reset");
sei();
}
