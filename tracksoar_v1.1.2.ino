/* trackuino copyright (C) 2010  EA5HAV Javi

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//**********************************************************************************
// Note: This trackuino software version has been modified for use with a Tracksoar
// Tracker by removing most everything not required for an ATmega328P processor and
// adding code for a number of additional Tracksoar features.
// These changes and additional features were made/supplied by: Barry Sloan, VE6SBS.
// For more info. see ttp://bear.sbszoo.com/construction/traker/Tracksoar/V1-1.htm
//**********************************************************************************
#define VERSION  "Tracksoar v1.1.2\n"

#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include "sensors_avr.h"
#include <Arduino.h>
#include <Wire.h>     // Not used here, but needed in this file for other code

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // deactivate internal pull-ups for twi

// For Time Slotted transmissions, the time spent to get_pos(), check_altitude and do an
// aprs_send must be accounted for to have the transmissions occur at the desired slot time
// and BUSY_TIME is the fudge factor used to account for this time which varies a few ms
// each transmission depending on the values involved with the calculations, but not enough
// to not transmit at the correct second. However using DEBUG statements increases the time
// enough that you may see transmission times occur 1 or 2 sec's later than expected and, of
// course, modifying the firmware can also through off the timing so don't forget to check
// that transmission times are correct after making firmware changes and adjust the BUSY_TIME
// as needed to have transmissions accur when they should. 
#define BUSY_TIME  1750    // (ms) A 1300 to 2200 ms duration works for this firmware version.


// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms.

// Module variables
int x = 0;
bool recent_transmission = false;
static int32_t next_aprs = 0;
static int32_t next_alt_check;

void setup()  // ----------------------------------------------------------------------------

// If PTT LED is used as a PTT indicator, turn PTT LED on when power is connected in order to
// confirm tracker is powered until the GPS gets lock and the GPS LED begins to flash.
{ pinMode(PTT_LED_PIN, OUTPUT);
  if (!PTT_LED_AS_PTT)
  { pin_write(PTT_LED_PIN, LOW);
  }
  else
  { pin_write(PTT_LED_PIN, HIGH);
  }

  cbi(PORTC, 4);    // deactivate internal pull-ups for twi
  cbi(PORTC, 5);

  Serial.begin(GPS_BAUDRATE);
  Serial.println();
  Serial.println(VERSION);

  afsk_setup();     // afsk.cpp
  gps_setup();      // gps.cpp
  sensors_setup();  //sensors_avr.cpp

  // If time slotted transmissions enabled, do not start until we get a valid time reference.
  if (APRS_SLOT >= 0)       // -1 disables time slotted transmissions
  { do
    { while (!Serial.available())
        power_save();
    }
    while (!gps_decode(Serial.read()));
    next_aprs = millis() - BUSY_TIME +
                1000 * (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else    // (if time slotted transmissions disabled)
  { next_aprs = millis();   // make 1st transmission when tracker is powered
  }

  next_alt_check =  next_aprs + 5000;   // 1st_alt_check after each transmission is 5 sec's later.

  if (PTT_LED_AS_PTT)
    pin_write(PTT_LED_PIN, LOW);  // GPS has lock so time to turn OFF the temporary PTT LED power indicator.
}

void get_pos()              // Get a valid position from the GPS
{ int valid_pos = 0;
  uint32_t timeout = millis();
  do
  { if (Serial.available())
      valid_pos = gps_decode(Serial.read());
  }
  while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;
}

void led_alarm()            // Visual Alarm Indicator for when there's a problem
{ while (1)                 // Rapidly flash the LED until problem is fixed and power has been cycled
  { pin_write(PTT_LED_PIN, LOW);
    delay(250);
    pin_write(PTT_LED_PIN, HIGH);
    delay(250);
  }
}

void loop()   // MAIN LOOP --------------------------------------------------------------------------
{ if ((int32_t) (millis() - next_aprs) >= 0)    // Time for another APRS frame
  {
#ifdef DEBUG_TIME_SLOT
    Serial.println();
    Serial.println(F("DEBUG_TIME_SLOT, TIME TO BEACON"));
    Debug_Time_Slot();
    Serial.println(F("          get_pos, check_altitude, aprs_send & calculate next_aprs & next_alt_check"));
#endif

    get_pos();
    check_altitude();      // Check Altitude for Peak and Burst
    aprs_send();
    if (APRS_SLOT >= 0)    // If time slotted transmissions enabled
    { next_aprs = millis() - BUSY_TIME +
                  1000 * (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
    }
    else    // (if time slotted transmissions disabled)
    { next_aprs += APRS_PERIOD * 1000L;
    }
    recent_transmission = true;
    next_alt_check = next_aprs - (APRS_PERIOD * 1000L) + 5000;       // next_alt_check is 5 sec's after a transmission

#ifdef DEBUG_TIME_SLOT
    Serial.println();
    Serial.print('[');
    Serial.print(millis());
    Serial.print(F("] recent_transmission="));
    Serial.print(recent_transmission);
    Serial.print(F(",  next_alt_check ="));
    Serial.print(next_alt_check);
    Serial.print(F(",  next_aprs = "));
    Serial.println(next_aprs);
#endif

    while (afsk_flush())
    { power_save();
    }
#ifdef DEBUG_MODEM
    afsk_debug();   // Show modem ISR stats from the previous transmission
#endif
  }
  if ((int32_t) (millis() - next_alt_check) >= 0)   // Time to Check for Maximum Altitude and Burst
  {
#ifdef DEBUG_TIME_SLOT
    Serial.println();
    Serial.println(F("DEBUG_TIME_SLOT, TIME TO CHECK FOR MAX ALTITUDE"));
    Debug_Time_Slot();
#endif

    get_pos();
    check_altitude();                               // Check Altitude for Peak and Burst
    finish_checking_altitude();                     // makes prev_alt_ft = alt_ft
    next_alt_check =  next_alt_check + 10000;       // alt_check every 10 sec's
    recent_transmission = false;

#ifdef DEBUG_TIME_SLOT
    Serial.println();
    Serial.println(F("DEBUG_TIME_SLOT - after get_pos, check_altitude, calculating next_alt_check"));
    Debug_Time_Slot();
#endif
  }
  power_save();     // Incoming GPS data or interrupts will wake us up
}

void Debug_Time_Slot()
{ Serial.print('[');
  Serial.print(millis());
  Serial.print(F("] gps_seconds:"));
  Serial.print(gps_seconds);
  Serial.print(F(", Period:"));
  Serial.print(APRS_PERIOD);
  Serial.print(F(", Slot:"));
  Serial.print(APRS_SLOT);
  Serial.print(F(", next_aprs:"));
  Serial.print(next_aprs);
  Serial.print(F("ms. next_alt_check:"));
  Serial.print(next_alt_check);
  Serial.println(F("ms."));
}
