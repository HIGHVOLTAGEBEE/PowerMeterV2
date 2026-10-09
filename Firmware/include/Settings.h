#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define MAGIC_NUMBER 0xAA55

struct AppSettings
{
  uint16_t magic;
  float maxCurrent;
  float maxVoltage;
  float minVoltage;
  float maxTemperature;
  float tripTime;
  bool outputEnabled;
  bool loggingEnabled;
  bool eFuseEnabled;
  bool defaultOutputOn;
  uint8_t language;
  uint8_t dimmerPercent;
  uint16_t pwmFrequency;
  char apSSID[32];
  char apPassword[32];
  uint16_t graphInterval;
  float vSmooth;
  float cSmooth;
  float calVScale;
  float calVOffset;
  float calAScale;
  float calAOffset;
  bool ledEnabled;
  bool buttonEnabled;
  uint8_t ledMaxBrightness;
  float logIntervalSec;
  bool ccMode;
  bool cpMode;
  float ccSetpoint;
  float cpSetpoint;
  bool mdnsEnabled;
  uint16_t crc;
};

class Settings
{
public:
  Settings();
  void begin();
  void load();
  void save();
  void reset();
  AppSettings data;
  bool valid;
};

#endif