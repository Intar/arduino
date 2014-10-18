This is a C++ library for Arduino for controlling an HD74800-compatible LCD in 4-bit mode, 
using a shiftregister with only 3 pins connected to Arduino.
Tested on Arduino 0010 Alpha.

Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/hardware/libraries folder.

When installed, this library should look like:

Arduino/lib/targets/libraries/LCD3Wire              (this library's folder)
Arduino/lib/targets/libraries/LCD3Wire/LCD3Wire.cpp  (the library implementation file)
Arduino/lib/targets/libraries/LCD3Wire/LCD3Wire.h    (the library description file)
Arduino/lib/targets/libraries/LCD3Wire/keywords.txt (the syntax coloring file)
Arduino/lib/targets/libraries/LCD3Wire/examples     (the examples in the "open" menu)
Arduino/lib/targets/libraries/LCD3Wire/readme.txt   (this file)

Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select LCD3Wire.  This will add a corresponding line to the top of your sketch:
#include <LCD3Wire.h>

To stop using this library, delete that line from your sketch.

Geeky information:
After a successful build of this library, a new file named "LCD3Wire.o" will appear
in "Arduino/hardware/libraries/LCD3Wire". This file is the built/compiled library
code.

If you choose to modify the code for this library (i.e. "LCD3Wire.cpp" or "LCD3Wire.h"),
then you must first 'unbuild' this library by deleting the "LCD3Wire.o" file. The
new "LCD3Wire.o" with your code will appear after the next press of "verify"
