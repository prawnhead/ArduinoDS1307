DS1307 Realtime Clock library for Arduino
by Christian Murphy, 2013-06-26, prawnhead@gmail.com
For the DS1307 RTC integrated circuit.
Data Sheet: https://www.sparkfun.com/datasheets/Components/DS1307.pdf
Written using the BOB-00099 breakout board from Sparkfun Electronics
https://www.sparkfun.com/products/99

To use this library:

	1. download RTC.h and RTC.cpp files and place them in the \libraries
 folder under your Arduino working directory (sketch folder).

	2. Refer to DS1307.ino for example code on using the RTC library.


General notes:
   The main sketch file must contain the following lines:
     #include <Wire.h> // above setup()
     Wire.begin(); // in setup(), before any call to RTC
   It is also useful to call rtc.begin() to read in to software the
     settings currently in use on the RTC hardware. Not essential.
   There is a limitation in the maximum data length that the DS1307 will
     allow when reading/writing data from/to NVRAM. Data reads have been
     cut into blocks of 32 bytes.

Connection Information:
   --------------------------------------------
   BOB-00099    Arduino Uno,    Arduino    Note
   pin          Duemilanove     Mega
                pin             pin
   --------------------------------------------
   SDA          A4              22         1
   SCL          A5              21         1
   SQW          2               2          1, 2
   GND          GND             GND
   5V           5V              5V
   --------------------------------------------
   Connection notes:
   1. Also connected to 5V via 10k resistor.
   2. Optional. See product documentation.

