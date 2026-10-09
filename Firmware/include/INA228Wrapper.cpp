#include "INA228Wrapper.h"

INA228Wrapper::INA228Wrapper() : _ina(0x40), _connected(false), _voltage(0), _current(0), _power(0),
  _temperature(0), _shuntVoltage(0), _energy(0), _charge(0),
  _voltageRaw(0), _currentRaw(0),
  _vAlpha(0.3f), _cAlpha(0.3f),
  _calVScale(1.0f), _calVOffset(0.0f), _calAScale(1.0f), _calAOffset(0.0f),
  _shuntRaw(0), _lastRead(0) {}

bool INA228Wrapper::begin(uint8_t sda, uint8_t scl) {
  Wire.begin(sda, scl);
  Wire.setClock(400000);

  if (!_ina.begin()) {
    _connected = false;
    return false;
  }

  _connected = _ina.isConnected();
  if (!_connected) return false;

  _ina.setMaxCurrentShunt(MAX_EXPECTED_CURRENT, SHUNT_RESISTOR);

  // Konfiguration deaktiviert: Alert-Pin (IO10) wird nicht verwendet,
  // Auslesung erfolgt zeitgesteuert im Loop (100 Hz)
  // Interne Wandlung: 4 Avg x 150us x 3 Kanaele (~550Hz) fuer genaue Werte
  _ina.setAverage(INA228_4_SAMPLES);
  _ina.setBusVoltageConversionTime(INA228_150_us);
  _ina.setShuntVoltageConversionTime(INA228_150_us);
  _ina.setTemperatureConversionTime(INA228_150_us);
  _ina.setMode(INA228_MODE_CONT_TEMP_BUS_SHUNT);

  delay(50);
  return true;
}

void INA228Wrapper::setSmoothing(float vAlpha, float cAlpha) {
  if (isnan(vAlpha) || isinf(vAlpha)) _vAlpha = 0.3f;
  else _vAlpha = constrain(vAlpha, 0.0f, 1.0f);
  if (isnan(cAlpha) || isinf(cAlpha)) _cAlpha = 0.3f;
  else _cAlpha = constrain(cAlpha, 0.0f, 1.0f);
}

void INA228Wrapper::setCalibration(float vScale, float vOffset, float aScale, float aOffset) {
  if (isnan(vScale) || isinf(vScale) || vScale <= 0.0f || vScale > 10.0f) vScale = 1.0f;
  if (isnan(vOffset) || isinf(vOffset) || vOffset < -100.0f || vOffset > 100.0f) vOffset = 0.0f;
  if (isnan(aScale) || isinf(aScale) || aScale <= 0.0f || aScale > 10.0f) aScale = 1.0f;
  if (isnan(aOffset) || isinf(aOffset) || aOffset < -100.0f || aOffset > 100.0f) aOffset = 0.0f;
  _calVScale = vScale;
  _calVOffset = vOffset;
  _calAScale = aScale;
  _calAOffset = aOffset;
}

void INA228Wrapper::resetSmoothing() {
  _voltage = 0;
  _current = 0;
  _power = 0;
}

bool INA228Wrapper::readAll() {
  if (!_connected) return false;

  _shuntVoltage = _ina.getShuntMilliVolt() * 0.001f;
  _shuntRaw = (int32_t)(_shuntVoltage / 0.0003125f);
  _voltageRaw = _ina.getBusVoltage();
  _currentRaw = _ina.getMilliAmpere() * 0.001f;
  float p = _ina.getMilliWatt() * 0.001f;
  _temperature = _ina.getTemperature();
  _energy = 0;
  _charge = 0;

  float vCal = _voltageRaw * _calVScale + _calVOffset;
  float aCal = _currentRaw * _calAScale + _calAOffset;

  if (_lastRead == 0) {
    _voltage = vCal;
    _current = aCal;
    _power = p;
  } else {
    _voltage += _vAlpha * (vCal - _voltage);
    _current += _cAlpha * (aCal - _current);
    _power += _vAlpha * (p - _power);
  }

  // Diagnose-Register lesen loescht interne Alert-Flags
  _ina.getDiagnoseAlert();

  _lastRead = millis();
  return true;
}

void INA228Wrapper::resetAccumulators() {
}

void INA228Wrapper::printDebug() {
  Serial.print("SHUNT_uV:");
  Serial.print(_shuntVoltage * 1000000.0f, 1);
  Serial.print(" V:");
  Serial.print(_voltage, 3);
  Serial.print(" A:");
  Serial.print(_current, 6);
  Serial.print(" W:");
  Serial.print(_power, 3);
  Serial.print(" T:");
  Serial.print(_temperature, 1);
  Serial.print(" Rsh:");
  Serial.print(SHUNT_RESISTOR, 6);
  Serial.print(" Imax:");
  Serial.print(MAX_EXPECTED_CURRENT, 1);
  Serial.println();
}
