#include "DataLogger.h"

DataLogger::DataLogger() : _enabled(false), _entryCount(0), _lastLogTime(0), _logIntervalSec(1.0f),
    _peakV(0), _peakA(0), _peakW(0), _minV(999), _minA(999) {}

bool DataLogger::begin() {
  if (!SPIFFS.begin(true)) return false;
  if (SPIFFS.exists(LOG_FILE)) {
    File f = SPIFFS.open(LOG_FILE, "r");
    if (f) {
      _entryCount = 0;
      uint8_t buf[1024];
      while (f.available()) {
        int n = f.read(buf, sizeof(buf));
        for (int i = 0; i < n; i++) {
          if (buf[i] == '\n') _entryCount++;
        }
      }
      f.close();
    }
  }
  return true;
}

void DataLogger::log(float v, float a, float w, float wh) {
  if (!_enabled) return;

  // Peaks/Minima seit dem letzten Log-Eintrag mitfuehren
  if (v > _peakV) _peakV = v;
  if (a > _peakA) _peakA = a;
  if (w > _peakW) _peakW = w;
  if (v < _minV && v > 0) _minV = v;
  if (a < _minA && a > 0) _minA = a;

  uint32_t now = millis();
  if (now - _lastLogTime < (uint32_t)(_logIntervalSec * 1000.0f)) return;
  _lastLogTime = now;

  File f = SPIFFS.open(LOG_FILE, "a");
  if (!f) return;

  f.printf("%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
    now / 1000, v, a, w, _peakV, _peakA, _peakW,
    (_minV > 900) ? 0.0f : _minV, (_minA > 900) ? 0.0f : _minA, wh);
  f.close();
  _entryCount++;

  _peakV = 0; _peakA = 0; _peakW = 0; _minV = 999; _minA = 999;

  if (_entryCount > MAX_LOG_ENTRIES) {
    File rf = SPIFFS.open(LOG_FILE, "r");
    File wf = SPIFFS.open("/tmp.csv", "w");
    if (rf && wf) {
      rf.readStringUntil('\n');
      while (rf.available()) {
        String line = rf.readStringUntil('\n');
        if (line.length() > 0) {
          wf.println(line);
        }
      }
      rf.close();
      wf.close();
      SPIFFS.remove(LOG_FILE);
      SPIFFS.rename("/tmp.csv", LOG_FILE);
      _entryCount--;
    }
  }
}

String DataLogger::readLog(int lines) {
  if (!SPIFFS.exists(LOG_FILE)) return "";
  File f = SPIFFS.open(LOG_FILE, "r");
  if (!f) return "";

  String result = "Timestamp,V,A,W,PeakV,PeakA,PeakW,MinV,MinA,Wh\n";
  int totalLines = 0;
  uint8_t buf[1024];
  while (f.available()) {
    int n = f.read(buf, sizeof(buf));
    for (int i = 0; i < n; i++) {
      if (buf[i] == '\n') totalLines++;
    }
  }
  f.close();

  f = SPIFFS.open(LOG_FILE, "r");
  if (!f) return result;

  int startLine = totalLines - lines;
  if (startLine < 0) startLine = 0;

  int currentLine = 0;
  while (f.available() && currentLine < startLine) {
    f.readStringUntil('\n');
    currentLine++;
  }

  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (line.length() > 0) {
      result += line + "\n";
    }
  }
  f.close();
  return result;
}

void DataLogger::clear() {
  SPIFFS.remove(LOG_FILE);
  _entryCount = 0;
}

// Liefert ab einer Byte-Position eine Anzahl Zeilen (fuer chunked Export).
// nextOffset = Position fuer den naechsten Aufruf, done = Dateiende erreicht.
String DataLogger::readLogChunk(uint32_t offset, int count, int32_t *nextOffset, bool *done) {
  *done = true;
  *nextOffset = -1;
  if (!SPIFFS.exists(LOG_FILE)) return "";
  File f = SPIFFS.open(LOG_FILE, "r");
  if (!f) return "";
  if (offset > 0 && !f.seek(offset)) { f.close(); return ""; }

  String out = "";
  int n = 0;
  while (f.available() && n < count) {
    String line = f.readStringUntil('\n');
    if (line.length() == 0) break;
    out += line;
    out += '\n';
    n++;
  }
  *nextOffset = (int32_t)f.position();
  *done = !f.available();
  f.close();
  return out;
}

uint32_t DataLogger::getEntryCount() {
  return _entryCount;
}

void DataLogger::logEvent(const char *tag) {
  if (!_enabled) return;
  File f = SPIFFS.open(LOG_FILE, "a");
  if (!f) return;
  f.printf("%lu,,,,,,,,,%s\n", millis() / 1000, tag);
  f.close();
  _entryCount++;
  _lastLogTime = millis();
  _peakV = 0; _peakA = 0; _peakW = 0; _minV = 999; _minA = 999;
}
