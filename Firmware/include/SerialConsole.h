#ifndef SERIALCONSOLE_H
#define SERIALCONSOLE_H

#include <Arduino.h>
#include "INA228Wrapper.h"
#include "DataLogger.h"
#include "Settings.h"
#include "StatusLED.h"
#include "WebInterface.h"

// JSON-Telemetry (100 Hz) + Kommando-Konsole ueber USB.
// Kommandos werden als JSON-Zeile gesendet, z.B.:
//   {"cmd":"status"}                     -> aktueller Zustand
//   {"cmd":"get"}                        -> alle Einstellungen
//   {"cmd":"set","key":"maxCurrent","value":25}
//   {"cmd":"output","value":1}           -> Output EIN/AUS
//   {"cmd":"dimmer","value":50}          -> PWM-Prozent
//   {"cmd":"dimmeren","value":1}         -> Dimmer-Modus EIN/AUS
//   {"cmd":"logging","value":1}
//   {"cmd":"efuse","value":1}
//   {"cmd":"resetefuse"}
//   {"cmd":"log"}                        -> Logdaten als CSV ausgeben
//   {"cmd":"logclear"}
//   {"cmd":"help"}
class SerialConsole
{
public:
  void begin(INA228Wrapper *sensor, DataLogger *logger, Settings *settings,
             StatusLED *led, WebInterface *web);
  // Im Haupt-Loop (Core 1) aufrufen: JSON-Telemetry 100 Hz, Status 0.2 Hz,
  // abgeschlossene serielle Kommandozeilen auswerten.
  void update();

private:
  INA228Wrapper *_sensor;
  DataLogger *_logger;
  Settings *_settings;
  StatusLED *_led;
  WebInterface *_web;

  uint32_t _lastTelemetry;
  uint32_t _lastStatus;

  void handleLine(const String &line);
  void printStatus();
  void printSettings();
  void setKeyValue(const String &key, const String &value);
  void dumpLog();
};

#endif
