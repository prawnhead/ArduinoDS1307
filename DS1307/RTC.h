/*
   ===================
     F I F T H B I T
     0 0 0 1 0 0 0 0
   ===================
   DS1307 Realtime Clock library for Arduino
   by Christian Murphy, 2013-06-26, chris@fifthbit.com
   For the DS1307 RTC integrated circuit.
   Data Sheet: https://www.sparkfun.com/datasheets/Components/DS1307.pdf
   Written using the BOB-00099 breakout board from Sparkfun Electronics
   https://www.sparkfun.com/products/99

   General notes:
   The main sketch file must contain the following lines:
     #include <Wire.h> // above setup()
     Wire.begin(); // in setup(), before any call to RTC
   It is also useful to call rtc.begin() to read in to software the
     settings currently in use on the RTC hardware. Not essential.
   There is a limitation in the maximum data length that the DS1307 will
     allow when reading/writing data from/to NVRAM. Data reads have been
     cut into blocks of 16 bytes.

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
*/
#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <Wire.h>

class RTC {
  private:
    const static byte DS1307_I2C_ADDRESS = 0x68;
    const static byte DS1307_NVRAM_BASE_ADDRESS = 0x00;
    const static byte DS1307_NVRAM_USER_BASE_ADDRESS = 0x08;
    const static byte DS1307_NVRAM_LAST_ADDRESS = 0x3F;
    const static byte DS1307_NVRAM_MAX_BLOCK_LENGTH = 0x20;
    const static byte DS1307_24_HR_TIME_MASK = 0x3F;
    const static byte DS1307_12_HR_TIME_MASK = 0x1F;
    const static byte DS1307_12HR_BIT = 0x40;
    const static byte DS1307_PM_BIT = 0x20;
    const static byte DS1307_SECONDS_MASK = 0x7F;
    const static byte DS1307_HALT_BIT = 0x80;
    const static byte DS1307_SQUARE_WAVE_BIT = 0x10;
    const static byte DS1307_SQUARE_WAVE_FREQ_MASK = 0x03;
    const static byte DS1307_SQUARE_WAVE_POLARITY_MASK = 0x80;
    const static byte DS1307_HOURS_REGISTER = 0x02;
    const static byte DS1307_CONTROL_REGISTER = 0x07;
  protected:
    byte currentControl;  // Control register value on DS1307
    byte nextControl;     // Control register to send to DS1307 on next set()
    boolean halted;       // True = clock halted
    boolean time24;       // True = 24hr time
  public:
    RTC();
    void begin();
    enum Frequency {oneHertz, fourKiloHertz, eightKiloHertz, thirtyTwoKiloHertz};
    void halt(boolean value);                     //
    void use24HourTime(boolean value);            //
    void useSquareWave(boolean value);            // All take effect next set()
    void setSquareWaveFrequency(Frequency freq);  //
    void setSquareWavePolarity(boolean high);     //
    boolean isHalted();
    boolean using24HourTime();
    boolean usingSquareWave();
    Frequency getSquareWaveFrequency(); 
    boolean getSquareWavePolarity();
    // Allows 12/24hr clock setting
    void set(byte second,        // 0-59    Set 12 hour time
             byte minute,        // 0-59
             byte hour,          // 1-23
             byte dayOfWeek,     // 1-7
             byte dayOfMonth,    // 1-28/29/30/31
             byte month,         // 1-12
             byte year,          // 0-99
             boolean pm);        // true = pm.
    // 24hr time sets clock to 24hr mode
    void set(byte second,        // 0-59    Set 24 hour time
             byte minute,        // 0-59
             byte hour,          // 1-23
             byte dayOfWeek,     // 1-7
             byte dayOfMonth,    // 1-28/29/30/31
             byte month,         // 1-12
             byte year);         // 0-99
    void get(byte &second,       //         Get current time
             byte &minute,
             byte &hour,
             byte &dayOfWeek,
             byte &dayOfMonth,
             byte &month,
             byte &year,
             boolean &time24hr,
             boolean &pm);
    void read(byte &value, byte offset);                // Read byte from user memory area
    void read(byte bytes[], byte count, byte offset);   // Read byte array from user memory area
    void readAny(byte &value, byte offset);             // Read any byte
    void write(byte value, byte offset);                // Write byte to user memory area
    void write(byte bytes[], byte count, byte offset);  // Write byte array to user memory
    inline byte decToBcd(byte val);
    inline byte bcdToDec(byte val);
};

#endif
