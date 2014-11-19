habner-g12
==========
This is my first attempt at an arduino APRS tracker. The board clearly needs some more work but its likely close
enough for testing purposes. It is designed to run the trackduino firmware, though this is untested. I will be including a BOM shortly, my goal for this board is to keep the cost below $75.
board requires 5v at 1amp (rough eestimate, will do testing to verify) to run. Next revision may include a 5v 
regulator, but for now I cant make it fit. I'm also looking at replacing the tqfp atmega with an mlf chip to save
space. Further space and weigh saving measures may include using a GPS chip antenna to replace the SMA connector,
using 2mm spaced pin headers or something similarly small, replacing the u-blox chip with a venus GPS chip, and 
replacing 0805 passives with something less manageable. 
revision 1 arduino based APRS tracker
