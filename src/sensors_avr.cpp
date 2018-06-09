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

#include "sensors_avr.hpp"
#include <Arduino.h>

#ifdef TRACKSOAR_20
	BME280 bme280;

	inline void setup_bme280()
	{
		bme280.settings.commInterface = I2C_MODE;
		bme280.settings.I2CAddress = 0x76;
		bme280.settings.runMode = 3; //Normal mode
		bme280.settings.tStandby = 0;
		bme280.settings.filter = 0;
		bme280.settings.tempOverSample = 1;
		bme280.settings.pressOverSample = 1;
		bme280.settings.humidOverSample = 1;

		if (!bme280.begin())
		{
			DEBUG_UART.println("Could not find a valid BME280 sensor, check wiring!");

			while (1) {}
		}
	}

	inline void setup_battery_adc()
	{

		ADCSRA = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz (the slowest it goes)

		ADMUX  = _BV(REFS0); // Set ADC reference to AVCC
		// No MUX values needed to be changed to use ADC0

		ADCSRB = 0;  // Set ADC to Free-Running Mode
		ACSR   = _BV(ADC);  // Turn off the analog comparator
		DIDR0  = _BV(BAT_ADC_PIN_IDX);   // Turn off digital input buffer on the bat in pin.

		ADCSRA |= _BV(ADEN);  // Enable ADC
		ADCSRA |= _BV(ADSC);  // Start a single conversion.
	}

	void sensors_setup()
	{
		setup_bme280();
		setup_battery_adc();
	}

	float sensors_temperature()
	{
		return bme280.readTempC();
	}

	int32_t sensors_pressure()
	{
		return (int32_t)bme280.readFloatPressure();
	}

	float sensors_humidity()
	{
		return bme280.readFloatHumidity();
	}

	// Trigger a conversion, wait for it to complete.
	// VRef is tied to avcc, so the actual
	// voltage is value * (avcc / bits)
	float sensors_battery()
	{
		ADCSRA |= _BV(ADSC);  // Start a single conversion.

		// Wait for it to convert
		while (ADCSRA & _BV(ADSC))
		{
			// Empty
		}

		return ADC * (AVCC_VOLTAGE / 1024);
	}

#else
	Adafruit_BMP085 bmp805;

	void sensors_setup()
	{
		if (!bmp805.begin())
		{
			DEBUG_UART.println("Could not find a valid BMP085 sensor, check wiring!");

			while (1) {}
		}
	}

	float sensors_temperature()
	{
		return bmp805.readTemperature();
	}

	int32_t sensors_pressure()
	{
		return bmp805.readPressure();
	}

	float sensors_humidity()
	{
		return SHT2x.GetHumidity();
	}

	uint16_t sensors_battery()
	{
		return 0xFFFF;
	}
#endif


#endif // ifdef AVR
