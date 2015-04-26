/* trackuino copyright (C) 2010  EA5HAV Javi
 * tracksoar sensor changes copyright (C) 2015 Nick Winters
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

#include <Wire.h>
#include "Adafruit_BMP085.h"
#include "SHT2x.h"

Adafruit_BMP085 bmp;
  
void sensors_setup() {
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}

float sensors_temperature() {
	return bmp.readTemperature();
}

int sensors_pressure() {
	return bmp.readPressure();
}

float sensors_humidity() {
	return SHT2x.GetHumidity();
}


#endif // ifdef AVR
