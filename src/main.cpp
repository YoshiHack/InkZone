#include <Arduino.h>

#include "inkzone/device_status.h"
#include "inkzone/settings.h"
#include "inkzone/sample_nfl_provider.h"

namespace {
constexpr unsigned long kSerialBaudRate = 115200;
constexpr unsigned long kHeartbeatIntervalMs = 30000;

inkzone::DeviceStatus deviceStatus;
inkzone::Settings settings;
inkzone::SampleNflProvider sampleProvider;
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


void printSampleGame() {
  constexpr int64_t kSampleStartUnix = 1767225600;
  constexpr int64_t kSampleEndUnix = kSampleStartUnix + 24 * 60 * 60;

  const inkzone::ProviderResponse response =
      sampleProvider.fetchGames(inkzone::League::kNfl,
                                kSampleStartUnix,
                                kSampleEndUnix);

  Serial.printf("Provider: %s\n", sampleProvider.name());
  Serial.printf("Provider result: %s\n",
                inkzone::providerResultName(response.result));

  if (response.result != inkzone::ProviderResult::kSuccess) {
    Serial.printf("Provider diagnostic: %s\n",
                  response.diagnostic.c_str());
    return;
  }

  for (const inkzone::Game& game : response.games) {
    Serial.printf("%s %d at %s %d\n",
                  game.away_team.abbreviation.c_str(),
                  game.away_score,
                  game.home_team.abbreviation.c_str(),
                  game.home_score);

    Serial.printf("Status: %s, clock: %s\n",
                  game.period.c_str(),
                  game.clock.c_str());
  }
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
  selectStateFromSettings();
  printDeviceState();
  printSampleGame();
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
