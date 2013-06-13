eepromTool-ds2433
=================

A tool for writing and reading OneWire based ds2433 eeproms.  

Install the Arduino OneWire library from here:
http://www.pjrc.com/teensy/td_libs_OneWire.html

Load the Arduino sketch, and upload it to your microcontroller.  

Wire up your arduino like so:

Ard Data Pin     Ard +5V   Ard Ground 
------------     -------   ---------- 
   |               |            |
   |     2.2k      |            |
   |----/\/\/\ -----            |
   |                            |
   |                            |
   |                            |
------------              ------------
OneWire Data              OneWire Grnd


Run the Python program with -h to see how to interact with your code:
$python tool.py -h

By default (with no flags) it will back up the eeprom to a folder named by its ROM. 
The file is timestamped with the linux epoch time of the data dump.  

If you run the code with the -l flag, it will rewrite the flash with the oldest 
recorded data dump.  This is useful for restoring the ds2433 to a previous state.  