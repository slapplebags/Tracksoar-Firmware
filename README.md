## Tracksoar Firmware
==========
This is a modified version of the [trackduino](https://github.com/trackuino/trackuino) firmware for the [Tracksoar](www.tracksoar.com) hardware.

### Configuring Hardware Version
This branch uses a super basic HAL and as a result must be told what version of hardware it is running on. [Edit the config here](https://github.com/slapplebags/Tracksoar-Firmware/blob/BME280-support/Tracksoar/Firmware/tracksoar/config.h#L30) to have the corect version uncommented. Everything before V1.2 should work as it normally did on the old set of sensors.

#### Build Status
[![CircleCI](https://circleci.com/gh/slapplebags/Tracksoar-Firmware/tree/master.svg?style=svg)](https://circleci.com/gh/slapplebags/Tracksoar-Firmware/tree/master)
