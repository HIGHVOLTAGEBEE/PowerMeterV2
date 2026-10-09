#ifndef INA228WRAPPER_H
#define INA228WRAPPER_H

#include <Arduino.h>
#include <Wire.h>
#include <INA228.h>

#define SHUNT_RESISTOR 0.0005f
#define MAX_EXPECTED_CURRENT 50.0f
#define INA228_ALERT_PIN 10

class INA228Wrapper {
public:
  INA228Wrapper();
  bool begin(uint8_t sda = 8, uint8_t scl = 9);
  bool readAll();

  float getVoltage() { return _voltage; }
  float getCurrent() { return _current; }
  float getPower() { return _power; }
  float getTemperature() { return _temperature; }
  float getShuntVoltage() { return _shuntVoltage; }
  float getShuntRaw() { return _shuntRaw; }
  float getEnergy() { return _energy; }
  float getCharge() { return _charge; }

  // unkalibrierte, ungeglättete Rohwerte (fuer Kalibrierung)
  float getVoltageRaw() { return _voltageRaw; }
  float getCurrentRaw() { return _currentRaw; }

  void setSmoothing(float vAlpha, float cAlpha);
  void setCalibration(float vScale, float vOffset, float aScale, float aOffset);
  void resetSmoothing();

  bool isConnected() { return _connected; }
  void resetAccumulators();
  void printDebug();

private:
  INA228 _ina;
  bool _connected;
  float _voltage, _current, _power, _temperature;
  float _shuntVoltage, _energy, _charge;
  float _voltageRaw, _currentRaw;
  float _vAlpha, _cAlpha;
  float _calVScale, _calVOffset, _calAScale, _calAOffset;
  int32_t _shuntRaw;
  uint32_t _lastRead;
};

#endif
