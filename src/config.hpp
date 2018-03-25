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

#ifndef __CONFIG_H__
#define __CONFIG_H__


// --------------------------------------------------------------------------
// THIS IS THE TRACKUINO FIRMWARE CONFIGURATION FILE. YOUR CALLSIGN AND
// OTHER SETTINGS GO HERE.
//
// NOTE: all pins are actual AVR based. Arduino crap is crap.
// http://www.arduino.cc/en/Hacking/PinMapping
// --------------------------------------------------------------------------

// Uncomment your Tracksoar hardware version here
//
#define TRACKSOAR_20

// --------------------------------------------------------------------------
// APRS config (aprs.c)
// --------------------------------------------------------------------------

// Set your callsign and SSID here. Common values for the SSID are
// (from http://zlhams.wikidot.com/aprs-ssidguide):
//
// - Balloons:  11
// - Cars:       9
// - Home:       0
// - IGate:      5
#define S_CALLSIGN      "KI6VBK"
#define S_CALLSIGN_ID   11

// Destination callsign: APRS (with SSID=0) is usually okay.
#define D_CALLSIGN      "APRS"
#define D_CALLSIGN_ID   0

// Digipeating paths:
// (read more about digipeating paths here: http://wa8lmf.net/DigiPaths/ )
// The recommended digi path for a balloon is WIDE2-1 or pathless. The default
// is pathless. Uncomment the following two lines for WIDE2-1 path:
#define DIGI_PATH1      "WIDE2"
#define DIGI_PATH1_TTL  1

// Disable WIDE pathing above this altitude (in metres)
// Set to something ridiculous (100,000) to disable.
#define DISABLE_PATH_ABOVE_ALT_METRES 3000

// APRS comment: this goes in the comment portion of the APRS message. You
// might want to keep this short. The longer the packet, the more vulnerable
// it is to noise.
#if defined(TRACKSOAR_20)
	#define APRS_COMMENT    "Tracksoar v2.0 Beta"
#else
	#error "No hardware version defined!"
#endif


// --------------------------------------------------------------------------
// AX.25 config (ax25.cpp)
// --------------------------------------------------------------------------

// TX delay in milliseconds
#define TX_DELAY      300

// --------------------------------------------------------------------------
// Tracker config (trackuino.pde)
// --------------------------------------------------------------------------

// APRS packets are slotted so that multiple trackers can be used without
// them stepping on one another. The transmission times are governed by
// the formula:
//
//         APRS_SLOT (seconds) + n * APRS_PERIOD (seconds)
//
// When launching multiple balloons, use the same APRS_PERIOD in all balloons
// and set APRS_SLOT so that the packets are spaced equally in time.
// Eg. for two balloons and APRS_PERIOD = 60, set APRS_SLOT to 0 and 30,
// respectively. The first balloon will transmit at 00:00:00, 00:01:00,
// 00:02:00, etc. and the second balloon will transmit at 00:00:30, 00:01:30,
// 00:02:30, etc.
#define APRS_SLOT     -1     // seconds. -1 disables slotted transmissions
#define APRS_PERIOD   60    // seconds
// #define APRS_PERIOD   5    // seconds

// GPS baud rate (in bits per second). This is also the baud rate at which
// debug data will be printed out the serial port.
#define GPS_BAUDRATE  9600

#if defined(__AVR_ATmega328P__)
	#pragma message "Debug serial on the 328 has some concerns. You should probably be sure it's disabled"
	#define DEBUG_UART Serial
	#define GPS_UART   Serial
#elif defined(__AVR_ATmega32U4__)
	#define DEBUG_UART Serial
	#define GPS_UART   Serial1
#else
	#error "Unknown platform!"
#endif  // __AVR_ATmega32U4__


// --------------------------------------------------------------------------
// Modem config (afsk.cpp)
// --------------------------------------------------------------------------

// Pre-emphasize the 2200 tone by 6 dB. This is actually done by
// de-emphasizing the 1200 tone by 6 dB and it might greatly improve
// reception at the expense of poorer FM deviation, which translates
// into an overall lower amplitude of the received signal. 1 = yes, 0 = no.
#define PRE_EMPHASIS    1


#if defined(__AVR_ATmega328P__)
	#error "Bug Connor to fix this!"
#elif defined(__AVR_ATmega32U4__)
	#define TX_ENABLE_DDR                 DDRD
	#define TX_ENABLE_PORT               PORTD
	#define TX_ENABLE_PIN_BIT                4

	#define TX_OUT_DDR                    DDRB
	#define TX_OUT_PORT                  PORTB
	#define TX_OUT_PIN_BIT                   6
#else
	#error "Unknown platform!"
#endif  // __AVR_ATmega32U4__


// --------------------------------------------------------------------------
// Sensors config (sensors.cpp)
// --------------------------------------------------------------------------

// Pin mappings for the internal / external temperature sensors. VS refers
// to (arduino) digital pins, whereas VOUT refers to (arduino) analog pins.
#define GPS_USING_UBLOX
#define MIN_NO_CYCLIC_SATS     1
// Units for temperature sensors (Added by: Kyle Crockett)
// 1 = Celsius, 2 = Kelvin, 3 = Fahrenheit
#define TEMP_UNIT 1

// The voltage of the AVCC rail. Basically the 5V rail.
// Nominally 4.99V, delta whatever the resistor tolerances
// are
#define AVCC_VOLTAGE 4.99

#if defined(__AVR_ATmega328P__)
	#error "Bug Connor to fix this!"
#elif defined(__AVR_ATmega32U4__)
	#define BAT_ADC_PIN_IDX                0
#else
	#error "Unknown platform!"
#endif  // __AVR_ATmega32U4__


// --------------------------------------------------------------------------
// Debug
// --------------------------------------------------------------------------

// This is the LED pin (PC6 - pin 6 of PORTC). The LED will be on while the AVR is
// running and off while it's sleeping, so its brightness gives an indication
// of the CPU activity.
#if defined(__AVR_ATmega328P__)
	#error "Bug Connor to fix this!"
#elif defined(__AVR_ATmega32U4__)
	#define LED_DDR                 DDRC
	#define LED_PORT               PORTC
	#define LED_PIN_BIT                6
#else
	#error "Unknown platform!"
#endif  // __AVR_ATmega32U4__

// Debug info includes printouts from different modules to aid in testing and
// debugging.
//
// Some of the DEBUG modes will cause invalid modulation, so do NOT forget
// to turn them off when you put this to real use.
//
// Particularly the DEBUG_AFSK will print every PWM sample out the serial
// port, causing extreme delays in the actual AFSK transmission.
//
// 1. To properly receive debug information, only connect the Arduino RX pin
//    to the GPS TX pin, and leave the Arduino TX pin disconnected.
//
// 2. On the serial monitor, set the baudrate to GPS_BAUDRATE (above),
//    usually 9600.
//
// 3. When flashing the firmware, disconnect the GPS from the RX pin or you
//    will get errors.

#define DEBUG_GPS    // GPS sentence dump and checksum validation
#define DEBUG_AX25   // AX.25 frame dump
#define DEBUG_MODEM  // Modem ISR overrun and profiling
// #define DEBUG_AFSK   // AFSK (modulation) output
#define DEBUG_RESET  // AVR reset
#define DEBUG_SENS   // Sensors


#endif

