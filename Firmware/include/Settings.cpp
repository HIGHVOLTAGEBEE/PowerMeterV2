#include "Settings.h"

// Schuetzt EEPROM-Zugriffe, da save() jetzt von beiden CPU-Kernen
// (Web-Task Core 0 / Haupt-Loop Core 1) aufgerufen werden kann.
static SemaphoreHandle_t _eepromMutex = NULL;
static void ensureEepromMutex()
{
  if (_eepromMutex == NULL)
  {
    _eepromMutex = xSemaphoreCreateMutex();
  }
}

Settings::Settings() : valid(false)
{
  data.magic = MAGIC_NUMBER;
  data.maxCurrent = 30.0f;
  data.maxVoltage = 60.0f;
  data.minVoltage = 0.0f;
  data.maxTemperature = 80.0f;
  data.tripTime = 1.0f;
  data.outputEnabled = false;
  data.loggingEnabled = false;
  data.eFuseEnabled = false;
  data.defaultOutputOn = false;
  data.language = 0;
  data.dimmerPercent = 100;
  data.pwmFrequency = 1000;
  strcpy(data.apSSID, "XT90_PowerMeter");
  strcpy(data.apPassword, "12345678");
  data.graphInterval = 0;
  data.vSmooth = 0.3f;
  data.cSmooth = 0.3f;
  data.calVScale = 1.0f;
  data.calVOffset = 0.0f;
  data.calAScale = 1.0f;
  data.calAOffset = 0.0f;
  data.ledEnabled = true;
  data.buttonEnabled = true;
  data.ledMaxBrightness = 50;
  data.logIntervalSec = 1.0f;
  data.ccMode = false;
  data.cpMode = false;
  data.ccSetpoint = 1.0f;
  data.cpSetpoint = 10.0f;
  data.mdnsEnabled = true;
  data.crc = 0;
}

void Settings::begin()
{
  EEPROM.begin(EEPROM_SIZE);
  load();
}

void Settings::load()
{
  uint8_t *ptr = (uint8_t *)&data;
  for (int i = 0; i < sizeof(AppSettings); i++)
  {
    ptr[i] = EEPROM.read(i);
  }
  if (data.magic == MAGIC_NUMBER)
  {
    uint16_t calcCrc = 0;
    for (int i = 0; i < sizeof(AppSettings) - 2; i++)
    {
      calcCrc += ptr[i];
    }
    valid = (calcCrc == data.crc);
    if (valid)
    {
      // ungueltige Float-Werte aus dem EEPROM korrigieren (sonst NaN in der Glättung)
      bool fixed = false;
      if (isnan(data.vSmooth) || isinf(data.vSmooth) || data.vSmooth < 0.0f || data.vSmooth > 1.0f) { data.vSmooth = 0.3f; fixed = true; }
      if (isnan(data.cSmooth) || isinf(data.cSmooth) || data.cSmooth < 0.0f || data.cSmooth > 1.0f) { data.cSmooth = 0.3f; fixed = true; }
      if (isnan(data.calVScale) || isinf(data.calVScale) || data.calVScale <= 0.0f || data.calVScale > 10.0f) { data.calVScale = 1.0f; fixed = true; }
      if (isnan(data.calAScale) || isinf(data.calAScale) || data.calAScale <= 0.0f || data.calAScale > 10.0f) { data.calAScale = 1.0f; fixed = true; }
      if (isnan(data.calVOffset) || isinf(data.calVOffset) || data.calVOffset < -100.0f || data.calVOffset > 100.0f) { data.calVOffset = 0.0f; fixed = true; }
      if (isnan(data.calAOffset) || isinf(data.calAOffset) || data.calAOffset < -100.0f || data.calAOffset > 100.0f) { data.calAOffset = 0.0f; fixed = true; }
      if (isnan(data.logIntervalSec) || isinf(data.logIntervalSec) || data.logIntervalSec < 0.1f || data.logIntervalSec > 3600.0f) { data.logIntervalSec = 1.0f; fixed = true; }
      if (isnan(data.ccSetpoint) || isinf(data.ccSetpoint) || data.ccSetpoint < 0.0f || data.ccSetpoint > 50.0f) { data.ccSetpoint = 1.0f; fixed = true; }
      if (isnan(data.cpSetpoint) || isinf(data.cpSetpoint) || data.cpSetpoint < 0.0f || data.cpSetpoint > 5000.0f) { data.cpSetpoint = 10.0f; fixed = true; }
      if (fixed) save();
    }
  }
  else
  {
    valid = false;
    reset();
  }
}

void Settings::save()
{
  ensureEepromMutex();
  xSemaphoreTake(_eepromMutex, portMAX_DELAY);
  uint8_t *ptr = (uint8_t *)&data;
  data.crc = 0;
  uint16_t calcCrc = 0;
  for (int i = 0; i < sizeof(AppSettings) - 2; i++)
  {
    calcCrc += ptr[i];
  }
  data.crc = calcCrc;
  for (int i = 0; i < sizeof(AppSettings); i++)
  {
    EEPROM.write(i, ptr[i]);
  }
  EEPROM.commit();
  valid = true;
  xSemaphoreGive(_eepromMutex);
}

void Settings::reset()
{
  data.magic = MAGIC_NUMBER;
  data.maxCurrent = 30.0f;
  data.maxVoltage = 60.0f;
  data.minVoltage = 0.0f;
  data.maxTemperature = 80.0f;
  data.tripTime = 1.0f;
  data.outputEnabled = false;
  data.loggingEnabled = false;
  data.eFuseEnabled = false;
  data.defaultOutputOn = false;
  data.language = 0;
  data.dimmerPercent = 100;
  data.pwmFrequency = 1000;
  strcpy(data.apSSID, "XT90_PowerMeter");
  strcpy(data.apPassword, "12345678");
  data.graphInterval = 0;
  data.vSmooth = 0.3f;
  data.cSmooth = 0.3f;
  data.calVScale = 1.0f;
  data.calVOffset = 0.0f;
  data.calAScale = 1.0f;
  data.calAOffset = 0.0f;
  data.ledEnabled = true;
  data.buttonEnabled = true;
  data.ledMaxBrightness = 50;
  data.logIntervalSec = 1.0f;
  data.ccMode = false;
  data.cpMode = false;
  data.ccSetpoint = 1.0f;
  data.cpSetpoint = 10.0f;
  data.mdnsEnabled = true;
  data.crc = 0;
  save();
}