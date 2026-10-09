#include "SerialConsole.h"

void SerialConsole::begin(INA228Wrapper *sensor, DataLogger *logger, Settings *settings,
                          StatusLED *led, WebInterface *web)
{
  _sensor = sensor;
  _logger = logger;
  _settings = settings;
  _led = led;
  _web = web;
  _lastTelemetry = 0;
  _lastStatus = 0;
}

// Sucht einen String- oder Zahlenwert zu einem Schluessel in einer JSON-Zeile.
static bool jsonGet(const String &line, const char *key, String &out)
{
  String pat = "\"";
  pat += key;
  pat += "\"";
  int i = line.indexOf(pat);
  if (i < 0) return false;
  i = line.indexOf(':', i + pat.length());
  if (i < 0) return false;
  i++;
  while (i < (int)line.length() && line[i] == ' ') i++;
  if (i < (int)line.length() && line[i] == '"')
  {
    int j = line.indexOf('"', i + 1);
    if (j < 0) return false;
    out = line.substring(i + 1, j);
  }
  else
  {
    int j = i;
    while (j < (int)line.length() && line[j] != ',' && line[j] != '}') j++;
    out = line.substring(i, j);
    out.trim();
  }
  return true;
}

void SerialConsole::update()
{
  // ---- Kommandos empfangen (zeilenweise) ----
  static String lineBuf;
  while (Serial.available())
  {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (lineBuf.length() > 0)
      {
        lineBuf.trim();
        handleLine(lineBuf);
        lineBuf = "";
      }
    }
    else if (lineBuf.length() < 200)
    {
      lineBuf += c;
    }
  }

  // ---- Telemetry 100 Hz (JSON) ----
  uint32_t ms = millis();
  if (ms - _lastTelemetry >= 10)
  {
    _lastTelemetry = ms;
    bool outputState = _web->getOutputEnabled() && !_web->getEFuseTriggered();
    Serial.printf("{\"t\":\"tel\",\"ms\":%lu,\"v\":%.3f,\"a\":%.4f,\"w\":%.3f,\"tmp\":%.1f,\"out\":%d,\"ef\":%d}\n",
                  ms, _sensor->getVoltage(), _sensor->getCurrent(), _sensor->getPower(),
                  _sensor->getTemperature(), outputState ? 1 : 0,
                  _web->getEFuseTriggered() ? 1 : 0);
  }

  // ---- Status alle 5 s (JSON) ----
  if (ms - _lastStatus >= 5000)
  {
    _lastStatus = ms;
    bool outputState = _web->getOutputEnabled() && !_web->getEFuseTriggered();
    Serial.printf("{\"t\":\"stat\",\"up\":%lu,\"out\":%d,\"ef\":%d,\"log\":%u,\"int\":%.1f,\"free\":%u,\"ws\":%u,\"ip\":\"%s\"}\n",
                  ms / 1000,
                  outputState ? 1 : 0,
                  _web->getEFuseTriggered() ? 1 : 0,
                  _logger->getEntryCount(),
                  _logger->getInterval(),
                  ESP.getFreeHeap(),
                  _web->getWSCount(),
                  WiFi.softAPIP().toString().c_str());
  }
}

void SerialConsole::handleLine(const String &line)
{
  String cmd;
  if (!jsonGet(line, "cmd", cmd))
  {
    Serial.println("{\"t\":\"err\",\"msg\":\"JSON erwartet, z.B. {\\\"cmd\\\":\\\"status\\\"} - siehe {\\\"cmd\\\":\\\"help\\\"}\"}");
    return;
  }

  if (cmd == "help")
  {
    Serial.println("{\"t\":\"help\",\"cmds\":[\"status\",\"get\",\"set\",\"output\",\"dimmer\",\"dimmeren\",\"logging\",\"efuse\",\"resetefuse\",\"log\",\"logclear\",\"help\"]}");
    Serial.println("{\"t\":\"help\",\"setkeys\":[\"maxCurrent\",\"maxVoltage\",\"minVoltage\",\"maxTemperature\",\"tripTime\",\"graphInterval\",\"vSmooth\",\"cSmooth\",\"calVScale\",\"calVOffset\",\"calAScale\",\"calAOffset\",\"ledEnabled\",\"ledMaxBrightness\",\"buttonEnabled\",\"logInterval\",\"ccMode\",\"cpMode\",\"ccSetpoint\",\"cpSetpoint\",\"defaultOutputOn\",\"language\",\"pwmFrequency\",\"dimmerPercent\",\"apSSID\",\"apPassword\",\"mdnsEnabled\"]}");
  }
  else if (cmd == "status")
  {
    printStatus();
  }
  else if (cmd == "get")
  {
    printSettings();
  }
  else if (cmd == "set")
  {
    String key, value;
    if (jsonGet(line, "key", key) && jsonGet(line, "value", value))
    {
      setKeyValue(key, value);
      _settings->save();
      _web->reloadHardwareSettings();
      Serial.println("{\"t\":\"ok\",\"cmd\":\"set\"}");
    }
    else
    {
      Serial.println("{\"t\":\"err\",\"msg\":\"set braucht key und value\"}");
    }
  }
  else if (cmd == "output")
  {
    String v;
    jsonGet(line, "value", v);
    _web->setOutputEnabled(v == "1" || v == "true");
    Serial.printf("{\"t\":\"ok\",\"cmd\":\"output\",\"out\":%d}\n", _web->getOutputEnabled() ? 1 : 0);
  }
  else if (cmd == "dimmer")
  {
    String v;
    jsonGet(line, "value", v);
    _web->setDimmerPercent((uint8_t)constrain(v.toInt(), 0, 100));
    Serial.printf("{\"t\":\"ok\",\"cmd\":\"dimmer\",\"dp\":%u}\n", _settings->data.dimmerPercent);
  }
  else if (cmd == "dimmeren")
  {
    String v;
    jsonGet(line, "value", v);
    _web->setDimmerEnabled(v == "1" || v == "true");
    Serial.println("{\"t\":\"ok\",\"cmd\":\"dimmeren\"}");
  }
  else if (cmd == "logging")
  {
    String v;
    jsonGet(line, "value", v);
    _logger->setEnabled(v == "1" || v == "true");
    _settings->data.loggingEnabled = _logger->isEnabled();
    _settings->save();
    Serial.println("{\"t\":\"ok\",\"cmd\":\"logging\"}");
  }
  else if (cmd == "efuse")
  {
    String v;
    jsonGet(line, "value", v);
    _settings->data.eFuseEnabled = (v == "1" || v == "true");
    _settings->save();
    Serial.println("{\"t\":\"ok\",\"cmd\":\"efuse\"}");
  }
  else if (cmd == "resetefuse")
  {
    _web->resetEFuse();
    Serial.println("{\"t\":\"ok\",\"cmd\":\"resetefuse\"}");
  }
  else if (cmd == "log")
  {
    dumpLog();
  }
  else if (cmd == "logclear")
  {
    _logger->clear();
    Serial.println("{\"t\":\"ok\",\"cmd\":\"logclear\"}");
  }
  else
  {
    Serial.printf("{\"t\":\"err\",\"msg\":\"unbekanntes cmd: %s\"}\n", cmd.c_str());
  }
}

void SerialConsole::printStatus()
{
  bool outputState = _web->getOutputEnabled() && !_web->getEFuseTriggered();
  Serial.printf("{\"t\":\"status\",\"v\":%.3f,\"a\":%.4f,\"w\":%.3f,\"tmp\":%.1f,\"wh\":%.3f,"
                "\"out\":%d,\"ef\":%d,\"eft\":%d,\"log\":%d,\"dp\":%u,\"de\":%d,"
                "\"cc\":%d,\"cp\":%d,\"ccs\":%.2f,\"cps\":%.1f}\n",
                _sensor->getVoltage(), _sensor->getCurrent(), _sensor->getPower(),
                _sensor->getTemperature(), _web->getEnergyAccum(),
                outputState ? 1 : 0,
                _settings->data.eFuseEnabled ? 1 : 0,
                _web->getEFuseTriggered() ? 1 : 0,
                _logger->isEnabled() ? 1 : 0,
                _settings->data.dimmerPercent,
                _web->getDimmerEnabled() ? 1 : 0,
                _settings->data.ccMode ? 1 : 0,
                _settings->data.cpMode ? 1 : 0,
                _settings->data.ccSetpoint,
                _settings->data.cpSetpoint);
}

void SerialConsole::printSettings()
{
  Serial.printf("{\"t\":\"settings\",\"maxCurrent\":%.2f,\"maxVoltage\":%.2f,\"minVoltage\":%.2f,"
                "\"maxTemperature\":%.1f,\"tripTime\":%.2f,\"graphInterval\":%u,"
                "\"vSmooth\":%.3f,\"cSmooth\":%.3f,"
                "\"calVScale\":%.5f,\"calVOffset\":%.4f,\"calAScale\":%.5f,\"calAOffset\":%.4f,"
                "\"ledEnabled\":%d,\"ledMaxBrightness\":%u,\"buttonEnabled\":%d,"
                "\"logInterval\":%.1f,\"loggingEnabled\":%d,\"defaultOutputOn\":%d,"
                "\"language\":%u,\"pwmFrequency\":%u,\"dimmerPercent\":%u,"
                "\"ccMode\":%d,\"cpMode\":%d,\"ccSetpoint\":%.2f,\"cpSetpoint\":%.1f,"
                "\"mdnsEnabled\":%d,"
                "\"apSSID\":\"%s\",\"apPassword\":\"%s\"}\n",
                _settings->data.maxCurrent, _settings->data.maxVoltage, _settings->data.minVoltage,
                _settings->data.maxTemperature, _settings->data.tripTime, _settings->data.graphInterval,
                _settings->data.vSmooth, _settings->data.cSmooth,
                _settings->data.calVScale, _settings->data.calVOffset,
                _settings->data.calAScale, _settings->data.calAOffset,
                _settings->data.ledEnabled ? 1 : 0, _settings->data.ledMaxBrightness,
                _settings->data.buttonEnabled ? 1 : 0,
                _settings->data.logIntervalSec, _settings->data.loggingEnabled ? 1 : 0,
                _settings->data.defaultOutputOn ? 1 : 0,
                _settings->data.language, _settings->data.pwmFrequency, _settings->data.dimmerPercent,
                _settings->data.ccMode ? 1 : 0, _settings->data.cpMode ? 1 : 0,
                _settings->data.ccSetpoint, _settings->data.cpSetpoint,
                _settings->data.mdnsEnabled ? 1 : 0,
                _settings->data.apSSID, _settings->data.apPassword);
}

void SerialConsole::setKeyValue(const String &key, const String &value)
{
  float f = value.toFloat();
  bool b = (value == "1" || value == "true");

  if (key == "maxCurrent") _settings->data.maxCurrent = f;
  else if (key == "maxVoltage") _settings->data.maxVoltage = f;
  else if (key == "minVoltage") _settings->data.minVoltage = f;
  else if (key == "maxTemperature") _settings->data.maxTemperature = f;
  else if (key == "tripTime") _settings->data.tripTime = f;
  else if (key == "graphInterval") _settings->data.graphInterval = (uint16_t)value.toInt();
  else if (key == "vSmooth") _settings->data.vSmooth = constrain(f, 0.0f, 1.0f);
  else if (key == "cSmooth") _settings->data.cSmooth = constrain(f, 0.0f, 1.0f);
  else if (key == "calVScale") _settings->data.calVScale = constrain(f, 0.1f, 10.0f);
  else if (key == "calVOffset") _settings->data.calVOffset = constrain(f, -100.0f, 100.0f);
  else if (key == "calAScale") _settings->data.calAScale = constrain(f, 0.1f, 10.0f);
  else if (key == "calAOffset") _settings->data.calAOffset = constrain(f, -100.0f, 100.0f);
  else if (key == "ledEnabled") { _settings->data.ledEnabled = b; _led->setEnabled(b); }
  else if (key == "ledMaxBrightness") { _settings->data.ledMaxBrightness = constrain(value.toInt(), 0, 100); _led->setMaxBrightness(_settings->data.ledMaxBrightness); }
  else if (key == "buttonEnabled") _settings->data.buttonEnabled = b;
  else if (key == "logInterval") _settings->data.logIntervalSec = constrain(f, 0.1f, 3600.0f);
  else if (key == "ccMode") { _settings->data.ccMode = b; if (b) _settings->data.cpMode = false; }
  else if (key == "cpMode") { _settings->data.cpMode = b; if (b) _settings->data.ccMode = false; }
  else if (key == "ccSetpoint") _settings->data.ccSetpoint = constrain(f, 0.0f, 50.0f);
  else if (key == "cpSetpoint") _settings->data.cpSetpoint = constrain(f, 0.0f, 5000.0f);
  else if (key == "defaultOutputOn") _settings->data.defaultOutputOn = b;
  else if (key == "language") _settings->data.language = (uint8_t)constrain(value.toInt(), 0, 1);
  else if (key == "pwmFrequency") _settings->data.pwmFrequency = (uint16_t)constrain(value.toInt(), 100, 30000);
  else if (key == "dimmerPercent") _web->setDimmerPercent((uint8_t)constrain(value.toInt(), 0, 100));
  else if (key == "apSSID") { strncpy(_settings->data.apSSID, value.c_str(), 31); _settings->data.apSSID[31] = 0; }
  else if (key == "apPassword") { strncpy(_settings->data.apPassword, value.c_str(), 31); _settings->data.apPassword[31] = 0; }
  else if (key == "mdnsEnabled") _settings->data.mdnsEnabled = b;
}

void SerialConsole::dumpLog()
{
  Serial.println("{\"t\":\"logstart\"}");
  Serial.println("Timestamp,V,A,W,PeakV,PeakA,PeakW,MinV,MinA,Wh");
  uint32_t offset = 0;
  bool done = false;
  while (!done)
  {
    int32_t next = -1;
    String chunk = _logger->readLogChunk(offset, 200, &next, &done);
    if (chunk.length() == 0) break;
    Serial.print(chunk);
    if (next < 0) break;
    offset = (uint32_t)next;
    vTaskDelay(pdMS_TO_TICKS(2)); // Watchdog & Puffer fuer USB-CDC
  }
  Serial.println("{\"t\":\"logend\"}");
}
