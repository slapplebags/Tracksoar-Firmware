#include "config.h"
#include "gps.h"
#include "nyanpollo.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module functions
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

void nyanpollo_send() {
  char data[12];

  Wire.begin();

  Wire.write(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  Wire.write("|");
  Wire.write(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  Wire.write("|");
  Wire.write(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  Wire.write("|");
  snprintf(data, 4, "%03d", (int)(gps_course + 0.5));           // Course (degrees)
  Wire.write(data);
  Wire.write("|");
  snprintf(data, 4, "%03d", (int)(gps_speed + 0.5));
  Wire.write(data);             // speed (knots)
  Wire.write("|");
  snprintf(data, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
  Wire.write(data);
  Wire.write("|");
  snprintf(data, 6, "%ld", sensors_pressure());
  Wire.write(data);
  Wire.write("|");
  dtostrf(sensors_humidity(), -1, 2, data);
  Wire.write(data);
  Wire.write("|");
  dtostrf(sensors_temperature(), -1, 2, data);
  Wire.write(data);

  Wire.end();
}
