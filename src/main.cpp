#include <Arduino.h>

#include "inkzone/device_status.h"
#include "inkzone/settings.h"
#include "inkzone/sample_nfl_provider.h"
#include "inkzone/espn_nfl_parser.h"
#include "inkzone/sample_nfl_json.h"
#include "inkzone/wifi_connection.h"
#include "inkzone/settings_web_server.h"
#include "inkzone/settings_storage.h"

namespace {
constexpr unsigned long kSerialBaudRate = 115200;
constexpr unsigned long kHeartbeatIntervalMs = 30000;

inkzone::DeviceStatus deviceStatus;
inkzone::Settings settings;
inkzone::SettingsWebServer settingsWebServer(settings, 90);
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
}

const char* gameStatusName(inkzone::GameStatus status) {
  switch (status) {
    case inkzone::GameStatus::kScheduled:
      return "scheduled";
    case inkzone::GameStatus::kLive:
      return "live";
    case inkzone::GameStatus::kHalftime:
      return "halftime";
    case inkzone::GameStatus::kDelayed:
      return "delayed";
    case inkzone::GameStatus::kFinal:
      return "final";
    case inkzone::GameStatus::kPostponed:
      return "postponed";
    case inkzone::GameStatus::kCanceled:
      return "canceled";
  }

  return "unknown";
}

void printParsedNflScoreboard() {
  const inkzone::ProviderResponse response =
      inkzone::parseEspnNflScoreboard(inkzone::kSampleNflScoreboardJson);

  Serial.printf("JSON parser result: %s\n",
                inkzone::providerResultName(response.result));

  if (response.result != inkzone::ProviderResult::kSuccess) {
    Serial.printf("JSON parser diagnostic: %s\n",
                  response.diagnostic.c_str());
    return;
  }

  for (const inkzone::Game& game : response.games) {
    Serial.printf("Parsed game: %s %d at %s %d\n",
                  game.away_team.abbreviation.c_str(),
                  game.away_score,
                  game.home_team.abbreviation.c_str(),
                  game.home_score);

Serial.printf("Parsed status: %s, period: %s, clock: %s\n",
              gameStatusName(game.status),
              game.period.c_str(),
              game.clock.c_str());

Serial.printf("Parsed start time: %lld\n",
              static_cast<long long>(game.scheduled_start_unix));
  }
}

void connectToWokwiWifi() {
  Serial.println("Connecting to Wokwi Wi-Fi...");

  const inkzone::WifiConnectResult result =
      inkzone::connectToWifi("Wokwi-GUEST", "", 10000);

  Serial.printf("Wi-Fi result: %s\n",
                inkzone::wifiConnectResultName(result));
                if (result == inkzone::WifiConnectResult::kSuccess) {
  settingsWebServer.begin();
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
  
  const bool settingsLoaded = inkzone::loadSettings(settings);

Serial.printf("Saved settings: %s\n",
              settingsLoaded ? "loaded" : "not found");
  connectToWokwiWifi();
  selectStateFromSettings();
  printDeviceState();
  printSampleGame();
  printParsedNflScoreboard();
}

void loop() {
  settingsWebServer.handleClient();
  const unsigned long nowMs = millis();

  if (nowMs - lastHeartbeatMs >= kHeartbeatIntervalMs) {
    lastHeartbeatMs = nowMs;
    Serial.printf("InkZone uptime: %lu seconds, state: %s\n", nowMs / 1000,
                  inkzone::deviceStateName(deviceStatus.state()));
  }

  delay(10);
}
