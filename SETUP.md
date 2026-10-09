# Setup Guide

Step-by-step instructions for building and setting up the XT90 Power Meter: flashing the firmware, first configuration and everyday operation.

The firmware is a **PlatformIO project for Visual Studio Code**. All board settings, dependencies and the flash partition table are defined in `platformio.ini` - you do not need to install any libraries manually.

---

## Table of Contents

- [1. Flashing the Firmware](#1-flashing-the-firmware)
- [2. First Boot](#2-first-boot)
- [3. First Configuration](#3-first-configuration)
- [4. Everyday Operation](#4-everyday-operation)
- [5. Calibration](#5-calibration)
- [6. Troubleshooting](#6-troubleshooting)

---

## 1. Flashing the Firmware

### 1.1 Requirements

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE extension](https://platformio.org/platformio-ide) for VS Code (when you open the project, VS Code also suggests the pioarduino fork via `Firmware/.vscode/extensions.json` - both work)
- USB-C cable (data capable)
- The `Firmware/` folder of this repository

The VS Code configuration in `Firmware/.vscode/extensions.json` additionally recommends the ESP exception decoder (`Jason2866.esp-decoder`), which is very helpful for decoding crash backtraces from the serial log.

### 1.2 Project Setup

1. Open VS Code and make sure the PlatformIO IDE extension is installed.
2. `PlatformIO Home > Open Project` and select the `Firmware/` folder.
3. PlatformIO automatically reads `platformio.ini` and installs all dependencies on the first build.

> **Note on the project layout:** this project builds with `src_dir = .` and keeps all firmware modules (`INA228Wrapper`, `WebInterface`, `WebPages`, `DataLogger`, `Settings`, `StatusLED`, `SerialConsole`) in `Firmware/include/` - that is intentional, no files need to be moved. Just open the `Firmware/` folder as the project root and build.

The project is preconfigured as follows (see `platformio.ini`):

| Setting | Value |
|---|---|
| Platform | `espressif32` |
| Board | `esp32-s3-devkitc-1` |
| Framework | Arduino |
| CPU frequency | 240 MHz |
| Flash | 8 MB, QIO mode, 80 MHz |
| Partition table | `partitions_8mb.csv` (3.5 MB app + 4 MB SPIFFS for the data logger) |
| USB CDC on boot | Enabled (serial console over USB) |
| Libraries | `mathieucarbou/ESPAsyncWebServer@^3.6.0`, `mathieucarbou/AsyncTCP@^3.3.2`, `robtillaart/INA228@^0.4.0` |
| Monitor speed | 2,000,000 baud |

### 1.3 Build and Upload

1. Connect the board via USB-C.
2. Click the **Build** (checkmark) and then the **Upload** (arrow) button in the PlatformIO toolbar at the bottom of VS Code.

Or via the command line:

```bash
cd Firmware
pio run                 # build
pio run --target upload # build and flash
```

If the upload does not start automatically, hold the boot button, re-plug the board (or press reset), and release the boot button to enter download mode.

> **Note:** The serial console runs at **2,000,000 baud**. `monitor_speed` is already set in `platformio.ini`, so `pio device monitor` (or the **Monitor** button in VS Code) opens the terminal at the correct rate.

---

## 2. First Boot

On first boot (or after a settings reset) the device:

1. Initializes the INA228 sensor (I2C, 400 kHz).
2. Mounts SPIFFS and counts existing log entries.
3. Starts the WiFi access point.
4. Restores all settings from EEPROM (falls back to defaults if invalid).

Default access point credentials:

| Parameter | Default |
|---|---|
| SSID | `XT90_PowerMeter` |
| Password | `12345678` |
| IP address | `192.168.4.1` |
| mDNS | `meter.local` (if enabled) |

The serial console prints a boot log including the AP IP address, the sensor status and the current settings.

---

## 3. First Configuration

Open the web interface and go to **Settings** (`/config`):

1. **WLAN:** change the AP SSID and password.
2. **System:** set the PWM frequency for your load (100 Hz - 30 kHz), default output state and language.
3. **Smoothing:** adjust the voltage/current IIR smoothing if the readings are too noisy or too slow (0 % = no smoothing).
4. **LED / Button:** configure the status LED brightness and whether the boot button is active.
5. **Logger:** set the log interval and auto-start behavior.

Press **Save** - all settings are stored in EEPROM with CRC validation and survive power cycles.

### Recommended eFuse Defaults

On the **eFuse** page, a sensible starting point is:

| Limit | Suggested value |
|---|---|
| Max current | 5 A below your continuous load rating |
| Max voltage | 10 % above your nominal input voltage |
| Min voltage | 10 % below your nominal input voltage |
| Max temperature | 80 °C |
| Trip delay | 0.5 - 1.0 s (longer for inrush, shorter for hard protection) |

Enable the eFuse with the **eFUSE ON** button. When a limit is exceeded for the configured delay, the output switches off and a trip report is displayed.

---

## 4. Everyday Operation

### Switching the Output

- Web interface (Live / eFuse / Dimmer page): the **OUTPUT** button.
- Boot button: short press toggles the output.
- Serial console: `{"cmd":"output","value":1}`.

### Dimmer and Regulators

1. Enable the dimmer on the **Dimmer** page.
2. Move the slider (throttled automatically to protect the WebSocket link), or
3. Enable a regulator:
   - **CC (constant current):** set a current in A; the P-controller adjusts the PWM duty cycle to hold it.
   - **CP (constant power):** set a power in W; the controller holds the power constant.

Regulators only run while the dimmer and the output are both enabled.

### Logging and Export

1. Enable logging on the **Logger** page.
2. Choose an interval (0.1 - 3600 s).
3. Export with **Excel Export** - the browser downloads the full log as an `.xls` file (HTML table format) with a progress dialog.
4. **Clear Log** wipes the log file.

### Status LED

| LED behavior | Meaning |
|---|---|
| Breathing (slow) | Output off, device idle |
| Solid on | Output on |
| Fast blinking | eFuse tripped |

---

## 5. Calibration

Use the calibration wizard at `/calibrate` for best accuracy:

1. Choose the channel (voltage or current).
2. Enter the reference value measured with a trusted instrument.
3. Apply a stable source and start the 3-second averaged measurement.
4. The wizard computes the calibration factor (reference / raw) and stores it in EEPROM.

Repeat for the second channel. Each factor is validated on boot and reset to 1.0 if it is out of range.

---

## 6. Troubleshooting

| Symptom | Possible cause | Solution |
|---|---|---|
| No AP visible | Wrong flash settings | Check that `partitions_8mb.csv` is used and USB CDC on boot is enabled (both set in `platformio.ini`), re-flash |
| Build fails on first try | Dependencies not downloaded yet | Run the build again - PlatformIO installs the libraries on first build |
| Web UI not loading | Browser cache | Hard reload (Ctrl+F5) |
| Sensor shows 0 / "INA228 not found" | I2C wiring, sensor damage | Check SDA/SCL (GPIO 8/9), 3.3 V supply |
| Readings noisy | Smoothing too low | Increase smoothing on the Settings page |
| Readings offset | Missing calibration | Run the calibration wizard |
| Output does not switch on | eFuse tripped / min voltage set | Check trip report on the eFuse page, reset eFuse |
| Logger full | SPIFFS capacity reached | Export and clear the log |
| Serial console garbled | Wrong baud rate | Use `pio device monitor` (2,000,000 baud is set in `platformio.ini`) |

If the issue persists, please open an issue with the serial boot log attached.
