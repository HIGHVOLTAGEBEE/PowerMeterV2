#include "WebInterface.h"
#include "WebPages.h"
#include <ESPmDNS.h>

WebInterface::WebInterface(INA228Wrapper *sensor, DataLogger *logger, Settings *settings, StatusLED *led)
    : _sensor(sensor), _logger(logger), _settings(settings), _led(led),
      _server(80), _ws("/ws"),
      _outputEnabled(false), _eFuseTriggered(false), _dimmerEnabled(false), _mdnsStarted(false),
      _lastBroadcast(0), _lastGraphUpdate(0),
      _graphIndex(0), _graphCount(0),
      _peakCurrent(0), _peakVoltage(0), _minCurrent(999), _minVoltage(999),
      _energyAccum(0), _startTime(0), _lastEnergyTime(0), _lastRegTime(0),
      _faultStartTime(0), _outputOnTime(0), _tripTime(0),
      _faultCurrent(0), _faultVoltage(0), _faultTemp(0), _faultReason("")
{
  memset(_graphData, 0, sizeof(_graphData));
  memset(_graphDataV, 0, sizeof(_graphDataV));
  memset(_graphDataW, 0, sizeof(_graphDataW));
}

void WebInterface::setOutputEnabled(bool en)
{
  _outputEnabled = en;
  applyDimmer();
}

void WebInterface::applyDimmer()
{
  if (_outputEnabled && !_eFuseTriggered && _dimmerEnabled)
  {
    int duty = (_settings->data.dimmerPercent * 1023) / 100;
    ledcWrite(PWM_PIN, duty);
  }
  else if (_outputEnabled && !_eFuseTriggered && !_dimmerEnabled)
  {
    ledcWrite(PWM_PIN, 1023);
  }
  else
  {
    ledcWrite(PWM_PIN, 0);
  }
}

void WebInterface::applyMdns()
{
  if (_settings->data.mdnsEnabled && !_mdnsStarted)
  {
    if (MDNS.begin("meter"))
    {
      MDNS.addService("http", "tcp", 80);
      _mdnsStarted = true;
    }
  }
  else if (!_settings->data.mdnsEnabled && _mdnsStarted)
  {
    MDNS.end();
    _mdnsStarted = false;
  }
}

void WebInterface::applyHardwareSettings()
{
  _sensor->setSmoothing(_settings->data.vSmooth, _settings->data.cSmooth);
  _sensor->setCalibration(_settings->data.calVScale, _settings->data.calVOffset,
                          _settings->data.calAScale, _settings->data.calAOffset);
  _led->setEnabled(_settings->data.ledEnabled);
  _led->setMaxBrightness(_settings->data.ledMaxBrightness);
  _logger->setIntervalSec(_settings->data.logIntervalSec);
}

void WebInterface::begin()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(_settings->data.apSSID, _settings->data.apPassword);
  applyMdns();

  ledcAttach(PWM_PIN, _settings->data.pwmFrequency, PWM_RES);

  _outputEnabled = _settings->data.defaultOutputOn;
  _dimmerEnabled = false;
  applyDimmer();
  applyHardwareSettings();

  _ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
              { onWsEvent(server, client, type, arg, data, len); });
  _server.addHandler(&_ws);

  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageLive()); });

  _server.on("/logger", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageLogger()); });

  _server.on("/efuse", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageEfuse()); });

  _server.on("/dimmer", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageDimmer()); });

  _server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageSettings()); });

  _server.on("/graph", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    int points = 100;
    if (request->hasParam("p")) {
      points = request->getParam("p")->value().toInt();
    }
    request->send(200, "text/plain", generateGraphData(points)); });

  _server.on("/loginfo", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    uint32_t total = _logger->getTotalBytes();
    uint32_t used = _logger->getUsedBytes();
    uint32_t pct = total > 0 ? (used * 100) / total : 0;
    String json = "{\"entries\":" + String(_logger->getEntryCount()) +
                  ",\"usedKB\":" + String(used / 1024) +
                  ",\"totalKB\":" + String(total / 1024) +
                  ",\"freeKB\":" + String((total - used) / 1024) +
                  ",\"pct\":" + String(pct) + "}";
    request->send(200, "application/json", json); });

  _server.on("/logchunk", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    uint32_t offset = 0;
    if (request->hasParam("o")) offset = request->getParam("o")->value().toInt();
    int count = 100;
    if (request->hasParam("c")) count = constrain(request->getParam("c")->value().toInt(), 1, 500);
    int32_t nextOffset;
    bool done;
    String data = _logger->readLogChunk(offset, count, &nextOffset, &done);
    String resp = "#OFF:" + String(nextOffset) + "," + (done ? "1" : "0") + "\n" + data;
    request->send(200, "text/plain", resp); });

  _server.on("/logclear", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    _logger->clear();
    request->send(200, "text/plain", "OK"); });

  _server.on("/resetefuse", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    _eFuseTriggered = false; _faultStartTime = 0; _outputOnTime = 0; _tripTime = 0;
    _faultReason = ""; _faultCurrent = 0; _faultVoltage = 0; _faultTemp = 0;
    _peakCurrent = 0; _peakVoltage = 0; _minCurrent = 999; _minVoltage = 999;
    _energyAccum = 0;
    request->send(200, "text/plain", "OK"); });

  _server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    String json = "{";
    json += "\"maxCurrent\":" + String(_settings->data.maxCurrent) + ",";
    json += "\"maxVoltage\":" + String(_settings->data.maxVoltage) + ",";
    json += "\"minVoltage\":" + String(_settings->data.minVoltage) + ",";
    json += "\"maxTemperature\":" + String(_settings->data.maxTemperature) + ",";
    json += "\"tripTime\":" + String(_settings->data.tripTime) + ",";
    json += "\"outputEnabled\":" + String(_outputEnabled ? "true" : "false") + ",";
    json += "\"loggingEnabled\":" + String(_logger->isEnabled() ? "true" : "false") + ",";
    json += "\"eFuseEnabled\":" + String(_settings->data.eFuseEnabled ? "true" : "false") + ",";
    json += "\"defaultOutputOn\":" + String(_settings->data.defaultOutputOn ? "true" : "false") + ",";
    json += "\"language\":" + String(_settings->data.language) + ",";
    json += "\"dimmerPercent\":" + String(_settings->data.dimmerPercent) + ",";
    json += "\"pwmFrequency\":" + String(_settings->data.pwmFrequency) + ",";
    json += "\"graphInterval\":" + String(_settings->data.graphInterval) + ",";
    json += "\"vSmooth\":" + String(_settings->data.vSmooth) + ",";
    json += "\"cSmooth\":" + String(_settings->data.cSmooth) + ",";
    json += "\"calVScale\":" + String(_settings->data.calVScale) + ",";
    json += "\"calAScale\":" + String(_settings->data.calAScale) + ",";
    json += "\"ledEnabled\":" + String(_settings->data.ledEnabled ? "true" : "false") + ",";
    json += "\"ledMaxBrightness\":" + String(_settings->data.ledMaxBrightness) + ",";
    json += "\"buttonEnabled\":" + String(_settings->data.buttonEnabled ? "true" : "false") + ",";
    json += "\"logInterval\":" + String(_settings->data.logIntervalSec) + ",";
    json += "\"ccMode\":" + String(_settings->data.ccMode ? "true" : "false") + ",";
    json += "\"cpMode\":" + String(_settings->data.cpMode ? "true" : "false") + ",";
    json += "\"ccSetpoint\":" + String(_settings->data.ccSetpoint) + ",";
    json += "\"cpSetpoint\":" + String(_settings->data.cpSetpoint) + ",";
    json += "\"apSSID\":\"" + String(_settings->data.apSSID) + "\",";
    json += "\"apPassword\":\"" + String(_settings->data.apPassword) + "\",";
    json += "\"mdnsEnabled\":" + String(_settings->data.mdnsEnabled ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json); });

  _server.on("/settings", HTTP_POST, [this](AsyncWebServerRequest *request)
             { handleSettingsPost(request); });

  _server.on("/calibrate", HTTP_GET, [this](AsyncWebServerRequest *request)
             { request->send(200, "text/html", webPageCalibrate()); });

  _server.on("/caldata", HTTP_GET, [this](AsyncWebServerRequest *request)
             {
    String json = "{\"v\":" + String(_sensor->getVoltageRaw(), 4) +
                  ",\"a\":" + String(_sensor->getCurrentRaw(), 5) +
                  ",\"sv\":" + String(_sensor->getVoltage(), 4) +
                  ",\"sa\":" + String(_sensor->getCurrent(), 5) + "}";
    request->send(200, "application/json", json); });

  _server.on("/calsave", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("chan", true) && request->hasParam("factor", true))
    {
      float factor = request->getParam("factor", true)->value().toFloat();
      if (factor > 0.1f && factor < 10.0f)
      {
        String chan = request->getParam("chan", true)->value();
        if (chan == "v")
        {
          _settings->data.calVScale = factor;
          _settings->data.calVOffset = 0.0f;
        }
        else if (chan == "a")
        {
          _settings->data.calAScale = factor;
          _settings->data.calAOffset = 0.0f;
        }
        _settings->save();
        applyHardwareSettings();
        _sensor->resetSmoothing();
        request->send(200, "text/plain", "OK");
        return;
      }
    }
    request->send(400, "text/plain", "ERR"); });

  _server.on("/output", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("state", true)) {
      _outputEnabled = request->getParam("state", true)->value() == "1";
      _settings->data.outputEnabled = _outputEnabled;
      _settings->save();
      applyDimmer();
    }
    request->send(200, "text/plain", "OK"); });

  _server.on("/dimmer", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("pct", true)) {
      _settings->data.dimmerPercent = constrain(request->getParam("pct", true)->value().toInt(), 0, 100);
      _settings->save();
      applyDimmer();
    }
    if (request->hasParam("en", true)) {
      _dimmerEnabled = request->getParam("en", true)->value() == "1";
      applyDimmer();
    }
    request->send(200, "text/plain", "OK"); });

  _server.on("/logging", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("state", true)) {
      _logger->setEnabled(request->getParam("state", true)->value() == "1");
      _settings->data.loggingEnabled = _logger->isEnabled();
      _settings->save();
    }
    request->send(200, "text/plain", "OK"); });

  _server.on("/efuse", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    if (request->hasParam("state", true)) {
      _settings->data.eFuseEnabled = request->getParam("state", true)->value() == "1";
      _settings->save();
    }
    request->send(200, "text/plain", "OK"); });

  _server.on("/reset", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
    _settings->reset();
    _outputEnabled = _settings->data.defaultOutputOn;
    _logger->setEnabled(_settings->data.loggingEnabled);
    _dimmerEnabled = false;
    ledcChangeFrequency(PWM_PIN, _settings->data.pwmFrequency, PWM_RES);
    applyHardwareSettings();
    applyDimmer();
    request->send(200, "text/plain", "OK"); });

  _server.begin();
  _startTime = millis();
  _lastEnergyTime = millis();
}

void WebInterface::runRegulator()
{
  if (!_settings->data.ccMode && !_settings->data.cpMode) return;
  if (!_outputEnabled || _eFuseTriggered || !_dimmerEnabled) return;

  uint32_t now = millis();
  if (_lastRegTime == 0) { _lastRegTime = now; return; }
  if (now - _lastRegTime < 100) return;
  float dt = (now - _lastRegTime) / 1000.0f;
  _lastRegTime = now;

  bool useCC = _settings->data.ccMode;
  float sp = useCC ? _settings->data.ccSetpoint : _settings->data.cpSetpoint;
  float meas = useCC ? _sensor->getCurrent() : _sensor->getPower();
  float err = sp - meas;

  // P-Regler: max +-4 Prozentpunkte pro Zyklus, Begrenzung auf 0..100 %
  float kp = 40.0f;
  float d = kp * err * dt;
  d = constrain(d, -4.0f, 4.0f);
  int pct = (int)_settings->data.dimmerPercent;
  pct += (int)(d >= 0.0f ? d + 0.5f : d - 0.5f);
  pct = constrain(pct, 0, 100);
  if (pct != (int)_settings->data.dimmerPercent)
  {
    _settings->data.dimmerPercent = (uint8_t)pct;
    applyDimmer();
  }
}

void WebInterface::handleSettingsPost(AsyncWebServerRequest *request)
{
  if (request->hasParam("maxCurrent", true)) {
    _settings->data.maxCurrent = request->getParam("maxCurrent", true)->value().toFloat();
  }
  if (request->hasParam("maxVoltage", true)) {
    _settings->data.maxVoltage = request->getParam("maxVoltage", true)->value().toFloat();
  }
  if (request->hasParam("minVoltage", true)) {
    _settings->data.minVoltage = request->getParam("minVoltage", true)->value().toFloat();
  }
  if (request->hasParam("maxTemperature", true)) {
    _settings->data.maxTemperature = request->getParam("maxTemperature", true)->value().toFloat();
  }
  if (request->hasParam("tripTime", true)) {
    _settings->data.tripTime = request->getParam("tripTime", true)->value().toFloat();
  }
  if (request->hasParam("graphInterval", true)) {
    _settings->data.graphInterval = request->getParam("graphInterval", true)->value().toInt();
  }
  if (request->hasParam("vSmooth", true)) {
    _settings->data.vSmooth = constrain(request->getParam("vSmooth", true)->value().toFloat(), 0.0f, 1.0f);
  }
  if (request->hasParam("cSmooth", true)) {
    _settings->data.cSmooth = constrain(request->getParam("cSmooth", true)->value().toFloat(), 0.0f, 1.0f);
  }
  if (request->hasParam("ledEnabled", true)) {
    _settings->data.ledEnabled = request->getParam("ledEnabled", true)->value() == "1";
  }
  if (request->hasParam("ledMaxBrightness", true)) {
    _settings->data.ledMaxBrightness = constrain(request->getParam("ledMaxBrightness", true)->value().toInt(), 0, 100);
  }
  if (request->hasParam("buttonEnabled", true)) {
    _settings->data.buttonEnabled = request->getParam("buttonEnabled", true)->value() == "1";
  }
  if (request->hasParam("logInterval", true)) {
    _settings->data.logIntervalSec = constrain(request->getParam("logInterval", true)->value().toFloat(), 0.1f, 3600.0f);
  }
  if (request->hasParam("ccMode", true)) {
    _settings->data.ccMode = request->getParam("ccMode", true)->value() == "1";
    if (_settings->data.ccMode) _settings->data.cpMode = false;
  }
  if (request->hasParam("cpMode", true)) {
    _settings->data.cpMode = request->getParam("cpMode", true)->value() == "1";
    if (_settings->data.cpMode) _settings->data.ccMode = false;
  }
  if (request->hasParam("ccSetpoint", true)) {
    _settings->data.ccSetpoint = constrain(request->getParam("ccSetpoint", true)->value().toFloat(), 0.0f, 50.0f);
  }
  if (request->hasParam("cpSetpoint", true)) {
    _settings->data.cpSetpoint = constrain(request->getParam("cpSetpoint", true)->value().toFloat(), 0.0f, 5000.0f);
  }
  if (request->hasParam("defaultOutputOn", true)) {
    _settings->data.defaultOutputOn = request->getParam("defaultOutputOn", true)->value() == "1";
  }
  if (request->hasParam("language", true)) {
    _settings->data.language = request->getParam("language", true)->value().toInt();
  }
  if (request->hasParam("pwmFrequency", true)) {
    _settings->data.pwmFrequency = constrain(request->getParam("pwmFrequency", true)->value().toInt(), 100, 30000);
    ledcChangeFrequency(PWM_PIN, _settings->data.pwmFrequency, PWM_RES);
  }
  if (request->hasParam("apSSID", true)) {
    strncpy(_settings->data.apSSID, request->getParam("apSSID", true)->value().c_str(), 31);
    _settings->data.apSSID[31] = 0;
  }
  if (request->hasParam("apPassword", true)) {
    strncpy(_settings->data.apPassword, request->getParam("apPassword", true)->value().c_str(), 31);
    _settings->data.apPassword[31] = 0;
  }
  if (request->hasParam("mdnsEnabled", true)) {
    _settings->data.mdnsEnabled = request->getParam("mdnsEnabled", true)->value() == "1";
  }
  _settings->save();
  applyHardwareSettings();
  applyMdns();
  applyDimmer();
  request->send(200, "text/plain", "OK");
}

void WebInterface::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                             AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("{\"t\":\"ws\",\"id\":%u,\"ev\":\"con\"}\n", client->id());
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("{\"t\":\"ws\",\"id\":%u,\"ev\":\"dis\"}\n", client->id());
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
      data[len] = 0;
      String msg = (char *)data;
      if (msg.startsWith("OUTPUT:"))
      {
        _outputEnabled = msg.substring(7) == "1";
        _settings->data.outputEnabled = _outputEnabled;
        _settings->save();
        applyDimmer();
      }
      else if (msg.startsWith("DIMMER:"))
      {
        String val = msg.substring(7);
        if (val.startsWith("EN:"))
        {
          _dimmerEnabled = val.substring(3) == "1";
          applyDimmer();
        }
        else
        {
          _settings->data.dimmerPercent = constrain(val.toInt(), 0, 100);
          _settings->save();
          applyDimmer();
        }
      }
      else if (msg.startsWith("LOG:"))
      {
        _logger->setEnabled(msg.substring(4) == "1");
        _settings->data.loggingEnabled = _logger->isEnabled();
        _settings->save();
      }
      else if (msg.startsWith("EFUSE:"))
      {
        _settings->data.eFuseEnabled = msg.substring(6) == "1";
        _settings->save();
      }
      else if (msg == "RESET_EFUSE")
      {
        _eFuseTriggered = false;
        _faultStartTime = 0;
        _outputOnTime = 0;
        _tripTime = 0;
        _faultReason = "";
        _faultCurrent = 0;
        _faultVoltage = 0;
        _faultTemp = 0;
        _peakCurrent = 0;
        _peakVoltage = 0;
        _minCurrent = 999;
        _minVoltage = 999;
        _energyAccum = 0;
      }
      else if (msg == "CLEAR_LOG")
      {
        _logger->clear();
      }
      else if (msg.startsWith("SET:"))
      {
        String params = msg.substring(4);
        int idx;
        while ((idx = params.indexOf(',')) > 0)
        {
          String pair = params.substring(0, idx);
          int sep = pair.indexOf('=');
          if (sep > 0)
          {
            String key = pair.substring(0, sep);
            float val = pair.substring(sep + 1).toFloat();
            if (key == "mc")
              _settings->data.maxCurrent = val;
            else if (key == "mv")
              _settings->data.maxVoltage = val;
            else if (key == "mnv")
              _settings->data.minVoltage = val;
            else if (key == "mt")
              _settings->data.maxTemperature = val;
            else if (key == "tt")
              _settings->data.tripTime = val;
            else if (key == "gi")
              _settings->data.graphInterval = (int)val;
            else if (key == "vs")
              _settings->data.vSmooth = constrain(val, 0.0f, 1.0f);
            else if (key == "cs")
              _settings->data.cSmooth = constrain(val, 0.0f, 1.0f);
            else if (key == "li")
              _settings->data.logIntervalSec = constrain(val, 0.1f, 3600.0f);
            else if (key == "ccs")
              _settings->data.ccSetpoint = constrain(val, 0.0f, 50.0f);
            else if (key == "cps")
              _settings->data.cpSetpoint = constrain(val, 0.0f, 5000.0f);
          }
          params = params.substring(idx + 1);
        }
        _settings->save();
        applyHardwareSettings();
      }
      else if (msg.startsWith("LED:"))
      {
        _settings->data.ledEnabled = msg.substring(4) == "1";
        _settings->save();
        _led->setEnabled(_settings->data.ledEnabled);
      }
      else if (msg.startsWith("LEDB:"))
      {
        _settings->data.ledMaxBrightness = constrain(msg.substring(5).toInt(), 0, 100);
        _settings->save();
        _led->setMaxBrightness(_settings->data.ledMaxBrightness);
      }
      else if (msg.startsWith("BTN:"))
      {
        _settings->data.buttonEnabled = msg.substring(4) == "1";
        _settings->save();
      }
    }
  }
}

void WebInterface::updateWeb()
{
  _ws.cleanupClients();

  uint32_t now = millis();
  if (now - _lastBroadcast >= 13)
  {
    _lastBroadcast = now;
    if (_ws.count() > 0)
    {
      broadcastData();
    }
  }
}

void WebInterface::updateSafety()
{
  uint32_t now = millis();

  float v = _sensor->getVoltage();
  float a = _sensor->getCurrent();
  float w = _sensor->getPower();
  float t = _sensor->getTemperature();

  if (a > _peakCurrent)
    _peakCurrent = a;
  if (v > _peakVoltage)
    _peakVoltage = v;
  if (a < _minCurrent && a > 0)
    _minCurrent = a;
  if (v < _minVoltage && v > 0)
    _minVoltage = v;

  uint32_t dt = now - _lastEnergyTime;
  if (dt > 0 && w > 0)
  {
    _energyAccum += w * (dt / 1000.0f) / 3600.0f;
  }
  _lastEnergyTime = now;

  if (now - _lastGraphUpdate >= 20)
  {
    _lastGraphUpdate = now;
    _graphData[_graphIndex] = a;
    _graphDataV[_graphIndex] = v;
    _graphDataW[_graphIndex] = w;
    _graphIndex = (_graphIndex + 1) % 600;
    if (_graphCount < 600)
      _graphCount++;
  }

  runRegulator();

  checkEFuse(a, v, t);
}

void WebInterface::broadcastData()
{
  float v = _sensor->getVoltage();
  float a = _sensor->getCurrent();
  float w = _sensor->getPower();
  float t = _sensor->getTemperature();
  uint32_t up = (millis() - _startTime) / 1000;

  String json = "{\"v\":" + String(v, 3) + ",\"a\":" + String(a, 3) +
                ",\"w\":" + String(w, 3) + ",\"t\":" + String(t, 1) +
                ",\"pv\":" + String(_peakVoltage, 3) + ",\"pa\":" + String(_peakCurrent, 3) +
                ",\"mv\":" + String(_minVoltage, 3) + ",\"ma\":" + String(_minCurrent, 3) +
                ",\"wh\":" + String(_energyAccum, 3) +
                ",\"out\":" + String(_outputEnabled ? "1" : "0") +
                ",\"log\":" + String(_logger->isEnabled() ? "1" : "0") +
                ",\"ef\":" + String(_settings->data.eFuseEnabled ? "1" : "0") +
                ",\"eft\":" + String(_eFuseTriggered ? "1" : "0") +
                ",\"up\":" + String(up) +
                ",\"fc\":" + String(_faultCurrent, 2) +
                ",\"fv\":" + String(_faultVoltage, 2) +
                ",\"ft\":" + String(_faultTemp, 1) +
                ",\"fr\":\"" + _faultReason + "\"" +
                ",\"ot\":" + String(_outputOnTime) +
                ",\"ts\":" + String(_tripTime) +
                ",\"dp\":" + String(_settings->data.dimmerPercent) +
                ",\"de\":" + String(_dimmerEnabled ? "1" : "0") +
                ",\"lc\":" + String(_logger->getEntryCount()) +
                ",\"cc\":" + String(_settings->data.ccMode ? "1" : "0") +
                ",\"cp\":" + String(_settings->data.cpMode ? "1" : "0") +
                ",\"lang\":" + String(_settings->data.language) + "}";

  _ws.textAll(json);
}

void WebInterface::checkEFuse(float current, float voltage, float temp)
{
  if (!_settings->data.eFuseEnabled)
    return;

  if (_eFuseTriggered)
  {
    _tripTime = (millis() - _faultStartTime) / 1000;
    return;
  }

  bool fault = false;
  String reason = "";

  if (current > _settings->data.maxCurrent && _settings->data.maxCurrent > 0)
  {
    fault = true;
    reason = (_settings->data.language == 0) ? "Überstrom" : "Overcurrent";
  }
  if (voltage > _settings->data.maxVoltage && _settings->data.maxVoltage > 0)
  {
    fault = true;
    reason = (_settings->data.language == 0) ? "Überspannung" : "Overvoltage";
  }
  if (voltage < _settings->data.minVoltage && _settings->data.minVoltage > 0 && voltage > 0)
  {
    fault = true;
    reason = (_settings->data.language == 0) ? "Unterspannung" : "Undervoltage";
  }
  if (temp > _settings->data.maxTemperature && _settings->data.maxTemperature > 0)
  {
    fault = true;
    reason = (_settings->data.language == 0) ? "Übertemperatur" : "Overtemp";
  }

  if (fault)
  {
    if (_faultStartTime == 0)
    {
      _faultStartTime = millis();
      _faultCurrent = current;
      _faultVoltage = voltage;
      _faultTemp = temp;
      _faultReason = reason;
      _outputOnTime = (millis() - _startTime) / 1000;
    }
    if (current > _faultCurrent)
      _faultCurrent = current;
    if (voltage > _faultVoltage)
      _faultVoltage = voltage;
    if (temp > _faultTemp)
      _faultTemp = temp;

    uint32_t tripDelayMs = (uint32_t)(_settings->data.tripTime * 1000.0f);
    if (tripDelayMs == 0)
      tripDelayMs = 100;

    if (millis() - _faultStartTime >= tripDelayMs)
    {
      _eFuseTriggered = true;
      _outputEnabled = false;
      _settings->data.outputEnabled = false;
      _settings->save();
      _tripTime = 0;
      applyDimmer();
    }
  }
  else
  {
    _faultStartTime = 0;
  }
}

String WebInterface::generateGraphData(int points)
{
  String data = "";
  int start = (_graphCount < 600) ? 0 : _graphIndex;
  int count = min(points, _graphCount);

  for (int i = 0; i < count; i++)
  {
    int idx = (start + i) % 600;
    if (i > 0)
      data += ",";
    data += String(_graphDataV[idx], 3) + ":" + String(_graphData[idx], 3) + ":" + String(_graphDataW[idx], 3);
  }
  return data;
}

