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

#include "sensors_avr.h"
#include <Arduino.h>

#ifdef TRACKSOAR_12
 	BME280 bme280;

	void sensors_setup() {
		bme280.settings.commInterface = I2C_MODE;
		bme280.settings.I2CAddress = 0x76;
		bme280.settings.runMode = 3; //Normal mode
		bme280.settings.tStandby = 0;
		bme280.settings.filter = 0;
		bme280.settings.tempOverSample = 1;
	    bme280.settings.pressOverSample = 1;
		bme280.settings.humidOverSample = 1;

	  if (!bme280.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	  }
	}

	float sensors_temperature() {
		return bme280.readTempC();
	}

	int32_t sensors_pressure() {
		return (int32_t)bme280.readFloatPressure();
	}

	float sensors_humidity() {
		return bme280.readFloatHumidity();
	}

#else
	Adafruit_BMP085 bmp805;

	void sensors_setup() {
	  if (!bmp805.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	  }
	}

	float sensors_temperature() {
		return bmp805.readTemperature();
	}

	int32_t sensors_pressure() {
		return bmp805.readPressure();
	}

	float sensors_humidity() {
		return SHT2x.GetHumidity();
	}
#endif


#endif // ifdef AVR
