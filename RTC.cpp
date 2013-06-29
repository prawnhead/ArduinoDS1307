#include "RTC.h"

RTC::RTC() {
  currentControl = 0;
  nextControl = 0;
  time24 = true;
}

void RTC::begin() {
  readAny(currentControl, DS1307_CONTROL_REGISTER);
  nextControl = currentControl;
  time24 = using24HourTime();
  halted = isHalted();
}

void RTC::halt(boolean value) {
  // takes effect on next set()
  halted = value;
}

void RTC::use24HourTime(boolean value) {
  // takes effect on next set()
  time24 = value;
}

void RTC::useSquareWave(boolean value) {
  // takes effect on next set()
  if (value) nextControl |= DS1307_SQUARE_WAVE_BIT;  // set bit 4, square wave on
  else nextControl &= DS1307_SQUARE_WAVE_BIT;        // clear bit 4, square wave off
}

void RTC::setSquareWaveFrequency(Frequency freq) {
  // takes effect on next set()
  switch (freq) {
    case oneHertz:
      nextControl &= 0xFC;  // clear bits 0, 1
      break;
    case fourKiloHertz:
      nextControl |= 0x01;  // set bit 0
      nextControl &= 0xFD;  // clear bit 1
      break;
    case eightKiloHertz:
      nextControl &= 0xFE;  // clear bit 0
      nextControl |= 0x02;  // set bit 1
      break;
    case thirtyTwoKiloHertz:
      nextControl |= 0x03;  // set bits 0, 1
      break;
  }
}

void RTC::setSquareWavePolarity(boolean high) {
  // takes effect on next set()
  if (high) nextControl &= 0x7F;  // bit 7 = 0 is output high
  else nextControl |= 0x80;       // bit 7 = 1 is output low
}

boolean RTC::isHalted() {
  // 1 on bit 7 of register 00H indicates clock halted
  byte reg;
  readAny(reg, DS1307_NVRAM_BASE_ADDRESS);
  return reg & DS1307_HALT_BIT;
}

boolean RTC::using24HourTime() {
  // 0 on bit 6 of register 02H indicates 24hr time
  byte reg;
  readAny(reg, DS1307_HOURS_REGISTER);
  return !(reg & DS1307_12HR_BIT);
}

boolean RTC::usingSquareWave() {
  // 1 on bit 4 of register 07H indicates square wave enabled
  return currentControl & DS1307_SQUARE_WAVE_BIT;
}

RTC::Frequency RTC::getSquareWaveFrequency() {
  // Bits 0, 1 of register 07H indicate square wave frequency
  switch (currentControl & DS1307_SQUARE_WAVE_FREQ_MASK) {
    case 0:
      return oneHertz;
      break;
    case 1:
      return fourKiloHertz;
      break;
    case 2:
      return eightKiloHertz;
      break;
    case 3:
      return thirtyTwoKiloHertz;
      break;
  }
}

boolean RTC::getSquareWavePolarity() {
  // 1 on bit 7 of register 07H indicates logic high output
  return currentControl & DS1307_SQUARE_WAVE_POLARITY_MASK;
}

// Sets the date and time in 24hr mode
void RTC::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  time24 = true;
  set(second, minute, hour, dayOfWeek, dayOfMonth, month, year, false);
}

void RTC::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year, boolean pm) {
  if (!time24 && hour > 12) {  // For safety. In case 12hr mode selected and 24hr set time given.
    pm = true;
    hour -= 12;
  }
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(DS1307_NVRAM_BASE_ADDRESS);
  if (halted) Wire.write(decToBcd(second) | DS1307_HALT_BIT);  // bit 7 = 1 halts the clock
  else Wire.write(decToBcd(second));                       // bit 7 = 0 starts the clock
  Wire.write(decToBcd(minute));
  if (time24) Wire.write(decToBcd(hour));                                          // bit 6 = 0 24hr time
  else {
    if (pm) Wire.write(decToBcd(hour) | DS1307_12HR_BIT | DS1307_PM_BIT);  // bit 6 = 1 12hr time, bit 5 = 1 PM
    else Wire.write(decToBcd(hour) | DS1307_12HR_BIT);                            // bit 6 = 1 12hr time, bit 5 = 0 AM
  }
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(nextControl);
  Wire.endTransmission();
  currentControl = nextControl;
}

// Gets the date and time from the DS1307
void RTC::get(byte &second, byte &minute, byte &hour, byte &dayOfWeek, byte &dayOfMonth, byte &month, byte &year, boolean &time24hr, boolean &pm)
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(DS1307_NVRAM_BASE_ADDRESS);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, (byte) 8);

  // A few of these need masks because certain bits are control bits
  second = bcdToDec(Wire.read() & DS1307_SECONDS_MASK);
  minute = bcdToDec(Wire.read());
  byte temp = Wire.read();
  if (temp & DS1307_12HR_BIT) {
    hour = bcdToDec(temp & DS1307_12_HR_TIME_MASK);  // 12 hour time
    pm = (temp & DS1307_PM_BIT);
    time24hr = false;
  } else {
    hour = bcdToDec(temp & DS1307_24_HR_TIME_MASK);  // 24 hour time
    pm = 0;
    time24hr = true;
  }
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
  currentControl = Wire.read();
}

void RTC::read(byte &value, byte offset)
{
  if ((DS1307_NVRAM_USER_BASE_ADDRESS + offset) > DS1307_NVRAM_LAST_ADDRESS) return;
  byte address = DS1307_NVRAM_USER_BASE_ADDRESS + offset;
  if (address > DS1307_NVRAM_LAST_ADDRESS) return;
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, (byte)1);
  value = Wire.read();
}

void RTC::read(byte bytes[], byte count, byte offset)
{
  if ((DS1307_NVRAM_USER_BASE_ADDRESS + offset) > DS1307_NVRAM_LAST_ADDRESS) return;
  byte index = 0; // index of writes to bytes[] array
  byte blockLength = DS1307_NVRAM_MAX_BLOCK_LENGTH;
  byte address = DS1307_NVRAM_USER_BASE_ADDRESS + offset;
  while (index < count) {
    if ((count - index) < blockLength) blockLength = count - index;
    if ((address + blockLength) > DS1307_NVRAM_LAST_ADDRESS) blockLength = DS1307_NVRAM_LAST_ADDRESS - address + 1;
    if (blockLength == 0) return;
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_I2C_ADDRESS, blockLength);
    for (int i = 0; i < blockLength; i++) {
      bytes[index++] = Wire.read();
    }
    address += blockLength;
  }
}

void RTC::readAny(byte &value, byte offset)
{
  byte address = offset;
  if (address > DS1307_NVRAM_LAST_ADDRESS) return;
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, (byte)1);
  value = Wire.read();
}

void RTC::write(byte value, byte offset) {
  if ((DS1307_NVRAM_USER_BASE_ADDRESS + offset) > DS1307_NVRAM_LAST_ADDRESS) return;
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(DS1307_NVRAM_USER_BASE_ADDRESS + offset);
  Wire.write(value);
  Wire.endTransmission();
}

void RTC::write(byte *bytes, byte count, byte offset)
{
  if ((DS1307_NVRAM_USER_BASE_ADDRESS + offset) > DS1307_NVRAM_LAST_ADDRESS) return;
  byte index = 0; // index of writes to bytes[] array
  byte blockLength = DS1307_NVRAM_MAX_BLOCK_LENGTH;
  byte address = DS1307_NVRAM_USER_BASE_ADDRESS + offset;
  while (index < count) {
    if ((count - index) < blockLength) blockLength = count - index;
    if ((address + blockLength) > DS1307_NVRAM_LAST_ADDRESS) blockLength = DS1307_NVRAM_LAST_ADDRESS - address + 1;
    if (!blockLength) return;
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(address);
    for (int i = 0; i < blockLength; i++) {
      Wire.write(bytes[index++]);
    }
    Wire.endTransmission();
    address += blockLength;
  }
}

byte RTC::decToBcd(byte val) {
  // Convert normal decimal numbers to binary coded decimal
  return ( (val / 10 * 16) + (val % 10) );
}

byte RTC::bcdToDec(byte val) {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val / 16 * 10) + (val % 16) );
}
