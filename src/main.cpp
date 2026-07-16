#include <Arduino.h>

#include "inkzone/device_status.h"
#include "inkzone/settings.h"

namespace {
constexpr unsigned long kSerialBaudRate = 115200;
constexpr unsigned long kHeartbeatIntervalMs = 30000;

inkzone::DeviceStatus deviceStatus;
inkzone::Settings settings;
unsigned long lastHeartbeatMs = 0;

void printBootInformation() {
  Serial.printf("Chip: %s, revision %d\n", ESP.getChipModel(),
                ESP.getChipRevision());
  Serial.printf("CPU: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash: %u bytes\n", ESP.getFlashChipSize());
}

void printDeviceState() {
  Serial.printf("State: %s\n", inkzone::deviceStateName(deviceStatus.state()));
}

void selectStateFromSettings() {
  const inkzone::SettingsError error = inkzone::validateSettings(settings);

  if (error == inkzone::SettingsError::kNone) {
    deviceStatus.transitionTo(inkzone::DeviceState::kConnecting, millis());
  } else {
    deviceStatus.transitionTo(inkzone::DeviceState::kNeedsSetup, millis());
    Serial.printf("Configuration: %s\n", inkzone::settingsErrorName(error));
  }
}
}  // namespace

void setup() {
  Serial.begin(kSerialBaudRate);
  delay(500);

  Serial.println();
  Serial.println("InkZone starting...");
  printBootInformation();
  selectStateFromSettings();
  printDeviceState();
}

void loop() {
  const unsigned long nowMs = millis();

  if (nowMs - lastHeartbeatMs >= kHeartbeatIntervalMs) {
    lastHeartbeatMs = nowMs;
    Serial.printf("InkZone uptime: %lu seconds, state: %s\n", nowMs / 1000,
                  inkzone::deviceStateName(deviceStatus.state()));
  }

  delay(10);
}
