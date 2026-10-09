#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <Arduino.h>
#include <SPIFFS.h>

#define LOG_FILE "/log.csv"
#define MAX_LOG_ENTRIES 65000

struct LogEntry {
  uint32_t timestamp;
  float voltage;
  float current;
  float power;
  float peakVoltage;
  float peakCurrent;
  float peakPower;
  float minVoltage;
  float minCurrent;
  float energy;
};

class DataLogger {
public:
  DataLogger();
  bool begin();
  // Peaks/Minima seit dem letzten Log-Eintrag mitfuehren
  void log(float v, float a, float w, float wh);
  // Markiere ein Ereignis (z.B. Neustart) als eigene Zeile im Log
  void logEvent(const char *tag);
  String readLog(int lines = 50);
  String readLogChunk(uint32_t offset, int count, int32_t *nextOffset, bool *done);
  uint32_t getUsedBytes() { return SPIFFS.usedBytes(); }
  uint32_t getTotalBytes() { return SPIFFS.totalBytes(); }
  void clear();
  uint32_t getEntryCount();
  bool isEnabled() { return _enabled; }
  void setEnabled(bool e) { _enabled = e; }
  float getInterval() { return _logIntervalSec; }
  void setIntervalSec(float s) { _logIntervalSec = (s < 0.1f) ? 0.1f : s; }

private:
  bool _enabled;
  uint32_t _entryCount;
  uint32_t _lastLogTime;
  float _logIntervalSec;
  float _peakV, _peakA, _peakW;
  float _minV, _minA;
};

#endif
