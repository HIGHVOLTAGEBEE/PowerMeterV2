#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "INA228Wrapper.h"
#include "DataLogger.h"
#include "Settings.h"
#include "StatusLED.h"

#define PWM_PIN 4
#define PWM_RES 10

class WebInterface
{
public:
  WebInterface(INA228Wrapper *sensor, DataLogger *logger, Settings *settings, StatusLED *led);
  void begin();
  // Laueft als eigener Task auf Core 0 (Webserver/Broadcast)
  void updateWeb();
  // Laueft im Haupt-Loop auf Core 1 (eFuse, Energie, Peaks, Graph, Regler)
  void updateSafety();
  void setOutputEnabled(bool en);
  bool getOutputEnabled() { return _outputEnabled; }
  bool getEFuseTriggered() { return _eFuseTriggered; }
  uint32_t getWSCount() { return _ws.count(); }
  float getEnergyAccum() { return _energyAccum; }
  void toggleOutput() { setOutputEnabled(!_outputEnabled); }
  void setDimmerPercent(uint8_t pct) { _settings->data.dimmerPercent = constrain(pct, 0, 100); applyDimmer(); }
  void setDimmerEnabled(bool en) { _dimmerEnabled = en; applyDimmer(); }
  bool getDimmerEnabled() { return _dimmerEnabled; }
  // Wendet Smoothing/Kalibrierung/Log-Intervall/LED-Einstellungen erneut an
  void reloadHardwareSettings() { applyHardwareSettings(); }
  void resetEFuse()
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

private:
  INA228Wrapper *_sensor;
  DataLogger *_logger;
  Settings *_settings;
  StatusLED *_led;
  AsyncWebServer _server;
  AsyncWebSocket _ws;

  bool _outputEnabled;
  bool _eFuseTriggered;
  bool _dimmerEnabled;
  bool _mdnsStarted;
  uint32_t _lastBroadcast;
  uint32_t _lastGraphUpdate;

  float _graphData[600];
  float _graphDataV[600];
  float _graphDataW[600];
  int _graphIndex;
  int _graphCount;

  float _peakCurrent, _peakVoltage, _minCurrent, _minVoltage;
  float _energyAccum;
  uint32_t _startTime;
  uint32_t _lastEnergyTime;
  uint32_t _lastRegTime;

  uint32_t _faultStartTime;
  uint32_t _outputOnTime;
  uint32_t _tripTime;
  float _faultCurrent, _faultVoltage, _faultTemp;
  String _faultReason;

  void broadcastData();
  String generateGraphData(int points);
  void checkEFuse(float current, float voltage, float temp);
  void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
  void applyDimmer();
  void applyMdns();
  void runRegulator();
  void applyHardwareSettings();
  void handleSettingsPost(AsyncWebServerRequest *request);
};

#endif
