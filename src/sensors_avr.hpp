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
#ifndef __SENSORS_AVR_H__
	#define __SENSORS_AVR_H__

	#include "config.hpp"
	#include <Wire.h>

	#ifdef TRACKSOAR_20
		#include "./SparkFunBME280.hpp"
		#pragma message("Version 2.0a")
	#else
		#error "Unknown version!"
	#endif

	void sensors_setup();
	float sensors_temperature();
	int32_t sensors_pressure();
	float sensors_humidity();
	float sensors_battery();

#endif // ifndef __SENSORS_AVR_H__
#endif // ifdef AVR
