#include "INA228Wrapper.h"
#include "DataLogger.h"
#include "WebInterface.h"
#include "Settings.h"
#include "StatusLED.h"
#include "SerialConsole.h"

#define BUTTON_PIN 0 // Boot Button, LOW aktiv
#define BTN_DEBOUNCE_MS 30
#define BTN_LONGPRESS_MS 2000

INA228Wrapper ina228;
DataLogger dataLogger;
Settings settings;
StatusLED statusLed;
WebInterface webInterface(&ina228, &dataLogger, &settings, &statusLed);
SerialConsole serialConsole;

uint32_t lastSensorRead = 0;

bool btnLastRaw = HIGH;
bool btnStableState = HIGH;
uint32_t btnLastChange = 0;
uint32_t btnPressStart = 0;
bool btnLongPressFired = false;

// Hinweis: Webserver/Broadcast laufen bewusst im Arduino-Haupt-Loop (Core 1),
// nicht in einem eigenen Core-0-Task. Der async_tcp-Task von AsyncTCP laeuft
// auf Core 0 - ein zweiter Task auf demselben Kern, der _ws.textAll()/
// cleanupClients() aufruft, wurde mitten in der Clientlisten-Iteration
// unterbrochen und hat den Webserver zum Absturz gebracht.
// updateSafety() (eFuse, Energie, Graph, Regler) bleibt als eigene Methode,
// damit die Trennung bei Bedarf leicht wieder aktiviert werden kann.

void setup()
{
  Serial.begin(2000000);
  delay(100);
  Serial.println("\nXT90 Power Meter starting...");

  settings.begin();

  if (!ina228.begin(8, 9))
  {
    Serial.println("INA228 not found!");
  }
  else
  {
    Serial.println("INA228 connected (400kHz I2C)");
    ina228.resetAccumulators();
    ina228.printDebug();
  }

  if (!dataLogger.begin())
  {
    Serial.println("SPIFFS failed!");
  }
  else
  {
    Serial.printf("SPIFFS ready, %u log entries\n", dataLogger.getEntryCount());
  }

  dataLogger.setEnabled(settings.data.loggingEnabled);
  dataLogger.setIntervalSec(settings.data.logIntervalSec);
  if (settings.data.loggingEnabled)
  {
    dataLogger.logEvent("RESTART"); // Neustart im Log vermerken
  }

  statusLed.begin(settings.data.ledEnabled, settings.data.ledMaxBrightness);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  webInterface.begin();

  serialConsole.begin(&ina228, &dataLogger, &settings, &statusLed, &webInterface);

  if (settings.data.defaultOutputOn)
  {
    webInterface.setOutputEnabled(true);
  }

  Serial.print("AP SSID: ");
  Serial.println(settings.data.apSSID);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Default Output: ");
  Serial.println(settings.data.defaultOutputOn ? "ON" : "OFF");
  Serial.print("Dimmer: ");
  Serial.print(settings.data.dimmerPercent);
  Serial.println("%");
  Serial.print("Serial Telemetry: 2000000 Baud, 100 Hz");
}

void handleButton()
{
  if (!settings.data.buttonEnabled)
    return;

  bool raw = digitalRead(BUTTON_PIN); // LOW = gedrueckt
  uint32_t now = millis();

  if (raw != btnLastRaw)
  {
    btnLastChange = now;
    btnLastRaw = raw;
  }

  if (now - btnLastChange < BTN_DEBOUNCE_MS)
    return;

  if (raw != btnStableState)
  {
    btnStableState = raw;
    if (raw == LOW)
    {
      // Taste gedrueckt
      btnPressStart = now;
      btnLongPressFired = false;
    }
    else
    {
      // Taste losgelassen
      if (!btnLongPressFired && now - btnPressStart < BTN_LONGPRESS_MS)
      {
        webInterface.toggleOutput();
        Serial.printf("{\"t\":\"btn\",\"out\":%d}\n", webInterface.getOutputEnabled() ? 1 : 0);
      }
    }
  }

  if (btnStableState == LOW && !btnLongPressFired && now - btnPressStart >= BTN_LONGPRESS_MS)
  {
    btnLongPressFired = true;
    webInterface.resetEFuse();
    Serial.println("{\"t\":\"btn\",\"efreset\":1}");
  }
}

void loop()
{
  uint32_t now = micros();

  // Auslesung zeitgesteuert: 100 Hz (Alert-Pin deaktiviert)
  if (now - lastSensorRead >= 10000)
  {
    lastSensorRead = now;
    ina228.readAll();
  }

  webInterface.updateWeb();
  webInterface.updateSafety();
  serialConsole.update();

  bool outputState = webInterface.getOutputEnabled() && !webInterface.getEFuseTriggered();
  statusLed.update(outputState, webInterface.getEFuseTriggered());

  handleButton();

  dataLogger.log(
      ina228.getVoltage(), ina228.getCurrent(), ina228.getPower(),
      webInterface.getEnergyAccum());
}
