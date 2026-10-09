#ifndef STATUSLED_H
#define STATUSLED_H

#include <Arduino.h>

#define STATUS_LED_PIN 5

class StatusLED
{
public:
  StatusLED();
  void begin(bool enabled, uint8_t maxBrightness);
  void setEnabled(bool en);
  void setMaxBrightness(uint8_t pct);
  void update(bool outputOn, bool efuseTripped);

private:
  bool _enabled;
  uint8_t _maxBrightness; // 0..100
  uint32_t _lastUpdate;
  bool _blinkOn;
  uint32_t _blinkLast;
};

#endif
