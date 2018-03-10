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

#ifndef __CONFIG_H__
#define __CONFIG_H__

// ---------------------------------------------------------------------------------
// THIS IS THE TRACKSOAR FIRMWARE CONFIGURATION FILE
// WHERE YOU ENTER YOUR CALLSIGN AND OTHER SETTINGS.
//
// NOTE: all pins are Arduino based, not the Atmega chip.
// Mapping: http://www.arduino.cc/en/Hacking/PinMapping
// ---------------------------------------------------------------------------------


// Set your CALLSIGN and SSID. The SSID for a Balloon is 11.
// For other common SSID values see http://zlhams.wikidot.com/aprs-ssidguide
#define S_CALLSIGN      "VE6ATV"
#define S_CALLSIGN_ID   11
// *************************************************


// Set DESTINATION CALLSIGN.
// The following (default), APRS (with SSID=0), is usually okay.
#define D_CALLSIGN      "APRS"
#define D_CALLSIGN_ID   0
// *************************************************


// DIGIPEATING PATH(s).
// (read more about digipeating paths here: http://wa8lmf.net/DigiPaths/ )
// ---------------------------------------------------------------------------------
// The recommended digi path for a balloon is NONE or WIDE2-1.
// Make DP_PATH = 0 for NONE, = 1 for a path of WIDE2-1 or enter an altitude (in Meters ASL)
// for a path of WIDE2-1 when the tracker is lower than the entered altitude and for NO PATH
// when higher than that altitude. Switching the path based on the trackers altitude is very
// useful as WIDE2-1 is often needed to reach an I-Gate when on or near the ground, but a
// WIDE2-1 path is very seldom required when higher than 1000 - 2000 meters and should not
// normally be used. (Note: The minimum altitude needed to enable path switching is 100 meters.)
#define DP_PATH  2000 // Enter 0, 1 or an altitude => 100 Meters ASL (Above Sea Level)
// *************************************************

// The following 2 entries provide the WIDE2-1 path when DP_PATH = 1 or an altitude
// => 100 Meters is entered and the tracker is at an altitude lower than that altitude.
// WIDE2-1 is the recommended path and should be used unless familiar with APRS path
// settings and you know the consequences of using something different.
#define DIGI_PATH1      "WIDE2"
#define DIGI_PATH1_TTL    1
// *************************************************


// APRS COMMENT. This is placed in the comment portion of the APRS message. It's best
// to keep this short as the longer the packet, the more vulnerable it is to noise.
#define APRS_COMMENT    "Tracksoar v1.1.2"
// *************************************************


// APRS PERIOD, or Transmit Rate, is how often, in seconds, a transmission will be made.
#define APRS_PERIOD   60    // sec's
// *************************************************

// APRS TIME, or SLOT TIME, is the specific "slot" of time in seconds past the top of the
// minute top of the minute that a transmission will occur when Time Slotted Transmissions 
// are enabled. To enable Time Slotted Transmissions, enter a time (in seconds) or enter -1
// to disable Time Slotted Transmissions.
#define APRS_SLOT      -1    // sec's. (-1 disables slotted transmissions) 
// *************************************************

// Time Slotted Transmissions are used to keep multiple trackers in a special event from
// transmitting at the same time. They are also useful when needing to know exactly when 
// to listen for a transmission, like when doing a grid search for a lost payload in hope
// of getting close enough to hear a transmission and trying to determine which of the 
// transmissions being heard may be from the lost payload and which are from elsewhere.
// 
// Time Slotted Transmission times are governed by the formula:
//                          APRS_SLOT (seconds) + n * APRS_PERIOD (seconds)
//
// When launching multiple balloons, use the same APRS_PERIOD for each balloon and set the 
// APRS_SLOT times so the packets are spaced equally in time. For example: for 2 balloons 
// and an APRS_PERIOD of 60 seconds, set the 1st balloons APRS_SLOT to 0 and the 2nd balloons
// APRS_SLOT to 30 to have
// the 1st balloon transmit at 00:00:00, 00:01:00, 00:02:00, etc. and to have
// the 2nd balloon transmit at 00:00:30, 00:01:30, 00:02:30, etc.
//
// BE CAREFUL when selecting the APRS_PERIOD as it can take up to an hour with some values for
// the tracker to begin transmitting in it's assigned slot. For help selecting an appropriate
// APRS_PERIOD and more information about Time Slotted Transmissions - visit:
// http://bear.sbszoo.com/construction/traker/Tracksoar/V1-1.htm#TimeSlottedTransmissions
//
// When Time Slotted Transmissions are Disabled, the first transmission occurs when the tracker
// is powered on and then continue at the Transmit Rate set by the APRS PERIOD entered. However,
// the time from the start of one transmission to the start of the next is NOT exactly the same
// as the entered APRS_PERIOD as the time it takes to process the code involved with updating the
// time for the next transmission is not taken into account which results in the duration actually
// being a few ms longer and, after an hour with an APRS_PERIOD of 60 seconds, transmissions will
// be found occuring about 8 sec's later each minute than they were an hour earlier.


// The PTT LED can be used as either a PTT indicator (that's on during a transmission) or
// as a processor activity indicator (that's on when the processor is awake, off when 
// it's asleep and therefore provides an indication of the CPU activity by its brightness).
// Enter 0 for a processor activity indicator and 1 (default) to make it a PTT indicator.
// As a PTT indicator it's also on and acts as a Power On indicator from the time power is
// applied until the GPS gets lock and the GPS LED begins to flash.
#define PTT_LED_AS_PTT   1
// *************************************************


// To reduce the packet length (and make it less vulnerable to noise), one can comment out
// any of the following lines to keep the corresponding data from being transmitted.
// (to comment out a line, add 2 slashes at the beginning of the line)
#define SEND_AR       // 4-8 char's - Ascent Rate
#define SEND_SATS     // 6-7 char's - Number of GPS Satellites in View
#define SEND_HDOP     //   9 char's - GPS HDOP Value
#define SEND_PA       //   8 char's - Atmospheric Pressure
#define SEND_RH       //   8 char's - Relative Humidity
#define SEND_TI       //   7 char's - Temperature Internal
#define SEND_MAX_ALT  //   9 char's - Maximum Altitude Reached. Adds B@xxxxxx (where xxxxxx is the
//                highest altitude reached) to the beginning of the comment when burst is detected.


// ***************** That's it!!!  Your Tracksoar is configured!!! ******************


// **********************************************************************************
// **********************************************************************************
// Following are a few more configuration entries, but some must not be changed and
// the others should only be changed if you are familiar with what they are for and
// know what needs to be entered.
// **********************************************************************************
// **********************************************************************************

// Tracker config (tracksoar.ino)
// ---------------------------------------------------------------------------------
#define GPS_BAUDRATE  9600    // (bits/second) Also debug data serial port baud rate.
#define PTT_LED_PIN     2

// GPS config (gps.cpp)
// ---------------------------------------------------------------------------------
#define GPS_USING_UBLOX
#define MIN_NO_CYCLIC_SATS     1

// Modem config
// ---------------------------------------------------------------------------------
// AUDIO_PIN is the audio-out pin. The audio is generated by timer 2 using PWM,
// so the only two options are pins 3 and 11. Pin 3 is used as Pin 11 doubles as
// MOSI and is used to interface with SPI devices.
#define AUDIO_PIN       3   // (afsk_avr.cpp) (radio_hx1.cpp)

// Pre-emphasize the 2200 tone by 6 dB. This is actually done by de-emphasizing
// the 1200 tone by 6 dB and it might greatly improve reception at the expense
// of poorer FM deviation which translates into an overall lower received signal
// amplitude. 1 = yes, 0 = no.
#define PRE_EMPHASIS    1   // (afsk.cpp)

// Radio config (radio_hx1.cpp)
// ---------------------------------------------------------------------------------
#define PTT_PIN         4

// AX.25 config (ax25.cpp)
// ---------------------------------------------------------------------------------
#define TX_DELAY      300   // TX delay in milliseconds

// Sensors config (sensors.cpp)
// ---------------------------------------------------------------------------------
// Most of the sensors.cpp functions use internal reference voltages (AVCC or 1.1V).
// To use an external reference, Uncomment the following line:
// #define USE_AREF

// BEWARE! YOU WILL FRY YOUR AVR if you connect an external voltage to the AREF pin
// and accidentally set the ADC to any of the internal references.
//
// It's always advised to connect the AREF pin through a pull-up resistor whose value
// is defined here in ohms (set to 0 if no pull-up):
// #define AREF_PULLUP           4700   // N/A to Tracksoar
//
// Since there is already a 32K resistor at the ADC pin, the actual voltage read will
// be VREF * 32 / (32 + AREF_PULLUP)
//
// Read more in the Arduino reference docs:
// http://arduino.cc/en/Reference/AnalogReference?from=Reference.AREF

// Voltage meter analog pin
// #define VMETER_PIN      2             // N/A to Tracksoar
// Resistor divider for the voltage meter (ohms)
// #define VMETER_R1       10000         // N/A to Tracksoar
// #define VMETER_R2       3300          // N/A to Tracksoar


// Debug:
// ---------------------------------------------------------------------------------
// Debug info includes printouts from different modules to aid in testing & debugging.
//
// Some of the DEBUG modes will cause invalid modulation and other issues, so DO NOT
// forget to turn them off before you put the tracker to real use.
//
// In particular, the DEBUG_AFSK will print every PWM sample out the serial port
// which causes extreme delays in the actual AFSK transmission. Delays introduced
// by DEBUG_GPS, DEBUG_AX25, DEBUG_MAX_ALTITUDE and other debug modes will also
// affect slotted transmission times.
//
// 1. On the serial monitor, set the baudrate to the GPS_BAUDRATE (above), usually 9600.
//
//  ***********************************************************************************
//  *  Item 1 in the trackuino software and the original tracksoar software was:      *
//  *  "To properly receive debug information, only connect the Arduino RX pin to the *
//  *  GPS TX pin and leave the Arduino TX pin disconnected". But I've never had a    *
//  *  problem receiving debug information using a FTDI USB to Serial Board with the  *
//  *  jumper in place and the Arduino TX pin connected to both, the GPS and FTDI,    *
//  *  RX pins. Also, with the jumper removed, the Arduino can't send configuration   *
//  *  data to the GPS which means a long wait each time the Arduino is reset and it  *
//  *  repeatedly tries (120 times) to configure the GPS before it finally continues  *
//  *  on. This, of course, also leaves the GPS un-configured.                        *
//  ***********************************************************************************
//
// 2. When flashing the firmware, disconnect the GPS TX pin from the Arduino RX pin
//    (by removing the jumper on the 2 pins nearest the 7 pin header) or you'll get
//    errors. Or, if you're like me and can never remember which jumper to remove, 
//    simply remove both. )

// #define DEBUG_AX25         // AX.25 frame dump
// #define DEBUG_MAX_ALTITUDE // Altitude and Burst Calculations
// #define DEBUG_TIME_SLOT    // Transmission Timing Debug Info
// #define DEBUG_GPS          // GPS sentence dump and checksum validation
// #define DEBUG_MODEM        // Modem ISR overrun and profiling
// #define DEBUG_AFSK         // AFSK (modulation) output
// #define DEBUG_SENS         // Sensors

#endif
