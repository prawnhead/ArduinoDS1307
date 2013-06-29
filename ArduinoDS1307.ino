#include "RTC.h"
#include <Wire.h>

RTC rtc;
int led = 13;
boolean ledMode = false;

void printDateTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year, boolean time24hr, boolean pm) {
  int fullYear = 2000 + year;
  Serial.print(year);
  Serial.print('-');
  if (month < 10) Serial.print('0');
  Serial.print(month);
  Serial.print('-');
  if (dayOfMonth < 10) Serial.print('0');
  Serial.print(dayOfMonth);
  Serial.print(' ');
  if (hour < 10) Serial.print('0');
  Serial.print(hour);
  Serial.print(':');
  if (minute < 10) Serial.print('0');
  Serial.print(minute);
  Serial.print(':');
  if (second < 10) Serial.print('0');
  Serial.print(second);
  Serial.print(" ");
  if (time24hr) Serial.print("24");
  else if (pm) Serial.print("PM");
  else Serial.print("AM");
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");
  Wire.begin();
  rtc.begin();
  rtc.setSquareWaveFrequency(RTC::oneHertz);
  rtc.useSquareWave(true);
  rtc.halt(false);
  rtc.set(0, 32, 13, 4, 27, 6, 13);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm;
  rtc.get(second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm);
  printDateTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm);
}

void loop() {
  Serial.println("loop()");
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm;
  rtc.get(second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm);
  printDateTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year, time24hr, pm);
  Serial.println();
  delay(1000);
}


