#include "StatusLED.h"

StatusLED::StatusLED() : _enabled(true), _maxBrightness(50),
  _lastUpdate(0), _blinkOn(false), _blinkLast(0) {}

void StatusLED::begin(bool enabled, uint8_t maxBrightness)
{
  _enabled = enabled;
  _maxBrightness = constrain(maxBrightness, 0, 100);
  ledcAttach(STATUS_LED_PIN, 5000, 8);
  ledcWrite(STATUS_LED_PIN, 0);
}

void StatusLED::setEnabled(bool en)
{
  _enabled = en;
  if (!en) ledcWrite(STATUS_LED_PIN, 0);
}

void StatusLED::setMaxBrightness(uint8_t pct)
{
  _maxBrightness = constrain(pct, 0, 100);
}

void StatusLED::update(bool outputOn, bool efuseTripped)
{
  uint32_t now = millis();

  if (!_enabled)
  {
    if (_lastUpdate != 0) ledcWrite(STATUS_LED_PIN, 0);
    _lastUpdate = 0;
    return;
  }

  // eFuse ausgeloest: schnelles Blinken (10 Hz)
  if (efuseTripped)
  {
    if (now - _blinkLast >= 100)
    {
      _blinkLast = now;
      _blinkOn = !_blinkOn;
      ledcWrite(STATUS_LED_PIN, _blinkOn ? (_maxBrightness * 255) / 100 : 0);
    }
    return;
  }

  // Output an: konstant an
  if (outputOn)
  {
    ledcWrite(STATUS_LED_PIN, (_maxBrightness * 255) / 100);
    return;
  }

  // Output aus: Breathing, 3s Zyklus, ~50 Hz Aktualisierung
  if (now - _lastUpdate < 20) return;
  _lastUpdate = now;

  float phase = (now % 3000) / 3000.0f * 2.0f * PI;
  float s = 0.5f - 0.5f * cosf(phase); // 0..1 sinusfoermig
  uint8_t duty = (uint8_t)(s * ((_maxBrightness * 255) / 100));
  ledcWrite(STATUS_LED_PIN, duty);
}
