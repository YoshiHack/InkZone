#include <Arduino.h>
#include <time.h>
#include <cstdlib>

#include "inkzone/device_status.h"
#include "inkzone/settings.h"
#include "inkzone/data_cache.h"
#include "inkzone/sample_nfl_provider.h"
#include "inkzone/espn_nfl_client.h"
#include "inkzone/espn_nfl_parser.h"
#include "inkzone/espn_ncaa_football_client.h"
#include "inkzone/espn_nba_client.h"
#include "inkzone/espn_ncaa_basketball_client.h"
#include "inkzone/espn_nhl_client.h"
#include "inkzone/sample_nfl_json.h"
#include "inkzone/wifi_connection.h"
#include "inkzone/settings_web_server.h"
#include "inkzone/settings_storage.h"
#include "inkzone/screen_selector.h"
#include "inkzone/refresh_scheduler.h"
#include "inkzone/simulator_display.h"
#include "inkzone/button_tracker.h"
#include "inkzone/view_mode_controller.h"

namespace {
const inkzone::Game* findNextFavoriteGame();
constexpr unsigned long kSerialBaudRate = 115200;
constexpr unsigned long kHeartbeatIntervalMs = 30000;

constexpr int kPreviousButtonPin = 32;
constexpr int kSelectButtonPin = 33;
constexpr int kNextButtonPin = 25;
constexpr size_t kManualViewCount = 3;

inkzone::ButtonTracker previousButton;
inkzone::ButtonTracker selectButton;
inkzone::ButtonTracker nextButton;

inkzone::ViewModeController viewModeController;

inkzone::DeviceStatus deviceStatus;
inkzone::Settings settings;
inkzone::SettingsWebServer settingsWebServer(settings, 90);
inkzone::SampleNflProvider sampleProvider;
unsigned long lastHeartbeatMs = 0;
unsigned long lastNflUpdateMs = 0;
inkzone::ProviderResponse cachedNflResponse;
bool hasCachedNflResponse = false;
inkzone::SportsDataCache scoreChangeCache;
inkzone::ActivityLevel currentActivity =
    inkzone::ActivityLevel::kNoGamesToday;

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

inkzone::ActivityLevel activityForScoreboard(
    const inkzone::ScreenDecision& decision,
    const inkzone::ProviderResponse& response,
    int64_t nowUnix) {
  if (decision.type == inkzone::ScreenType::kLiveFavorite) {
    return inkzone::ActivityLevel::kLiveFavoriteGame;
  }

  if (decision.type == inkzone::ScreenType::kLiveLeague) {
    return inkzone::ActivityLevel::kOtherLiveGame;
  }

  if (decision.type == inkzone::ScreenType::kUpcomingFavorite) {
    return inkzone::ActivityLevel::kGameWithinHour;
  }

  if (decision.type == inkzone::ScreenType::kFavoriteSchedule) {
    return inkzone::ActivityLevel::kGamesToday;
  }

  bool hasGameToday = false;

  for (const inkzone::Game& game : response.games) {
    const int64_t secondsUntilStart =
        game.scheduled_start_unix - nowUnix;

    if (secondsUntilStart >= 0 && secondsUntilStart <= 60 * 60) {
      return inkzone::ActivityLevel::kGameWithinHour;
    }

    if (secondsUntilStart >= 0 &&
        secondsUntilStart <= 24 * 60 * 60) {
      hasGameToday = true;
    }
  }

  return hasGameToday
             ? inkzone::ActivityLevel::kGamesToday
             : inkzone::ActivityLevel::kNoGamesToday;
}

inkzone::ProviderResponse fetchConfiguredLeagueScoreboard() {
  const inkzone::League selectedLeague =
      settings.favorite_leagues.empty()
          ? inkzone::League::kNfl
          : settings.favorite_leagues.front();

  switch (selectedLeague) {
    case inkzone::League::kNcaaFootball:
      return inkzone::fetchEspnNcaaFootballScoreboard();

    case inkzone::League::kNba:
      return inkzone::fetchEspnNbaScoreboard();

    case inkzone::League::kNcaaBasketball:
      return inkzone::fetchEspnNcaaBasketballScoreboard();

    case inkzone::League::kNhl:
      return inkzone::fetchEspnNhlScoreboard();

    case inkzone::League::kNfl:
    case inkzone::League::kUnknown:
    case inkzone::League::kMls:
    case inkzone::League::kSoccer:
      return inkzone::fetchEspnNflScoreboard();
  }

  return inkzone::fetchEspnNflScoreboard();
}

void printLiveNflScoreboard() {
  Serial.println("Requesting selected league scoreboard...");

inkzone::ProviderResponse response =
    fetchConfiguredLeagueScoreboard();
    std::vector<inkzone::ScoreChange> scoreChanges;

  Serial.printf(
      "Selected league result: %s\n",
      inkzone::providerResultName(response.result));

  if (response.result != inkzone::ProviderResult::kSuccess) {
  Serial.printf(
      "Live NFL diagnostic: %s\n",
      response.diagnostic.c_str());

  if (hasCachedNflResponse) {
    Serial.println("Using most recent valid NFL scoreboard...");
    response = cachedNflResponse;
  } else {
    Serial.println("Using saved scoreboard fallback...");

    response =
        inkzone::parseEspnNflScoreboard(
            inkzone::kSampleNflScoreboardJson);

    if (response.result != inkzone::ProviderResult::kSuccess) {
      Serial.println("Saved NFL scoreboard fallback failed");
      return;
    }
  }
} else {
  inkzone::SportsDataSnapshot snapshot;
snapshot.provider_name = "Selected ESPN Scoreboard";
snapshot.updated_at_unix =
    static_cast<int64_t>(time(nullptr));
snapshot.games = response.games;

scoreChanges =
    scoreChangeCache.findScoreChanges(snapshot);

scoreChangeCache.storeIfValid(snapshot);
  cachedNflResponse = response;
  hasCachedNflResponse = true;
  Serial.println("Cached latest valid scoreboard");
}

const std::string* favoriteTeamIds =
    settings.favorite_team_ids.empty()
        ? nullptr
        : settings.favorite_team_ids.data();

const int64_t nowUnix =
    static_cast<int64_t>(time(nullptr));

const inkzone::ScreenDecision screenDecision =
    inkzone::chooseAutomaticScreen(
        response.games.data(),
        response.games.size(),
        favoriteTeamIds,
        settings.favorite_team_ids.size(),
        nowUnix);

currentActivity =
    activityForScoreboard(
        screenDecision,
        response,
        nowUnix);

Serial.printf(
    "Recommended refresh: %lu seconds\n",
    static_cast<unsigned long>(
        inkzone::refreshIntervalMs(currentActivity) / 1000UL));

Serial.printf(
    "Automatic screen: %s\n",
    inkzone::screenTypeName(screenDecision.type));
    if (scoreChanges.empty() && !response.games.empty()) {
  const inkzone::Game& game = response.games.front();

  const inkzone::ScoreChange demoChange = {
      game.id,
      game.league,
      game.home_team,
      game.home_score,
      game.home_score + 1,
  };

}
    for (const inkzone::ScoreChange& scoreChange : scoreChanges) {
  inkzone::renderSimulatorScoreAlert(scoreChange);
}
    if (screenDecision.type ==
        inkzone::ScreenType::kUpcomingFavorite &&
    screenDecision.featured_game != nullptr) {
  inkzone::renderSimulatorUpcomingGame(
      *screenDecision.featured_game);
} else if (screenDecision.type ==
           inkzone::ScreenType::kIdleDashboard) {
  inkzone::renderSimulatorIdleDashboard(
    findNextFavoriteGame(),
    nowUnix,
    favoriteTeamIds,
    settings.favorite_team_ids.size());
} else {
  inkzone::renderSimulatorScoreboard(
      response,
      favoriteTeamIds,
      settings.favorite_team_ids.size());
}

  Serial.printf(
      "Scoreboard games received: %u\n",
      static_cast<unsigned int>(response.games.size()));

  for (const inkzone::Game& game : response.games) {
    Serial.printf(
        "Scoreboard game: %s %d at %s %d\n",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);
  }
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

bool synchronizeClock() {
  Serial.println("Synchronizing clock...");

  const char* timezoneRule = "UTC0";

if (settings.timezone == "America/New_York") {
  timezoneRule = "EST5EDT,M3.2.0/2,M11.1.0/2";
} else if (settings.timezone == "America/Chicago") {
  timezoneRule = "CST6CDT,M3.2.0/2,M11.1.0/2";
} else if (settings.timezone == "America/Denver") {
  timezoneRule = "MST7MDT,M3.2.0/2,M11.1.0/2";
} else if (settings.timezone == "America/Los_Angeles") {
  timezoneRule = "PST8PDT,M3.2.0/2,M11.1.0/2";
}

setenv("TZ", timezoneRule, 1);
tzset();

configTime(0, 0, "pool.ntp.org");

  const unsigned long startedMs = millis();
  constexpr unsigned long kTimeSyncTimeoutMs = 10000;

  time_t now = time(nullptr);

  while (now < 1000000000 &&
         millis() - startedMs < kTimeSyncTimeoutMs) {
    delay(100);
    now = time(nullptr);
  }

  if (now < 1000000000) {
    Serial.println("Clock synchronization timed out");
    return false;
  }

  Serial.printf(
      "Clock synchronized: %lld\n",
      static_cast<long long>(now));

  return true;
}

void connectToConfiguredWifi() {
#if defined(INKZONE_WOKWI_SIMULATOR)
  if (settings.wifi_ssid.empty()) {
    settings.wifi_ssid = "Wokwi-GUEST";
    settings.wifi_password.clear();
  }
#endif

  if (settings.wifi_ssid.empty()) {
    Serial.println("No saved Wi-Fi network.");
    Serial.println("Starting InkZone setup network...");

    if (inkzone::startSetupAccessPoint("InkZone-Setup")) {
      Serial.println("Setup network started: InkZone-Setup");
      Serial.println("Open http://192.168.4.1:90");
      settingsWebServer.begin();
    } else {
      Serial.println("Unable to start setup network");
    }

    return;
  }

  Serial.println("Connecting to saved Wi-Fi...");

  const inkzone::WifiConnectResult result =
      inkzone::connectToWifi(
          settings.wifi_ssid.c_str(),
          settings.wifi_password.c_str(),
          10000);

  Serial.printf(
      "Wi-Fi result: %s\n",
      inkzone::wifiConnectResultName(result));

  if (result == inkzone::WifiConnectResult::kSuccess) {
    synchronizeClock();
    settingsWebServer.begin();
    return;
  }

  Serial.println("Saved Wi-Fi failed.");
  Serial.println("Starting InkZone setup network...");

  if (inkzone::startSetupAccessPoint("InkZone-Setup")) {
    Serial.println("Setup network started: InkZone-Setup");
    Serial.println("Open http://192.168.4.1:90");
    settingsWebServer.begin();
  } else {
    Serial.println("Unable to start setup network");
  }
}

bool isFavoriteTeam(
    const inkzone::Team& team) {
  for (const std::string& favorite :
       settings.favorite_team_ids) {
    if (team.id == favorite ||
        team.abbreviation == favorite) {
      return true;
    }
  }

  return false;
}

const inkzone::Game* findNextFavoriteGame() {
  const inkzone::Game* nextGame = nullptr;

  for (const inkzone::Game& game :
       cachedNflResponse.games) {
    const bool isFavorite =
        isFavoriteTeam(game.home_team) ||
        isFavoriteTeam(game.away_team);

    if (!isFavorite ||
        game.status != inkzone::GameStatus::kScheduled) {
      continue;
    }

    if (nextGame == nullptr ||
        game.scheduled_start_unix <
            nextGame->scheduled_start_unix) {
      nextGame = &game;
    }
  }

  return nextGame;
}

void renderManualView() {
  if (cachedNflResponse.games.empty()) {
    return;
  }

  const std::string* favoriteTeamIds =
      settings.favorite_team_ids.empty()
          ? nullptr
          : settings.favorite_team_ids.data();

  const size_t selectedView =
      viewModeController.selectedView();

  const inkzone::Game* nextGame =
      findNextFavoriteGame();

  if (selectedView == 1 && nextGame != nullptr) {
    inkzone::renderSimulatorUpcomingGame(*nextGame);
    inkzone::renderSimulatorModeLabel("MANUAL");
    return;
  }

  if (selectedView == 2) {
    inkzone::renderSimulatorIdleDashboard(
    nextGame,
    static_cast<int64_t>(time(nullptr)),
    favoriteTeamIds,
    settings.favorite_team_ids.size());
    inkzone::renderSimulatorModeLabel("MANUAL");
    return;
  }

  inkzone::renderSimulatorScoreboard(
      cachedNflResponse,
      favoriteTeamIds,
      settings.favorite_team_ids.size());
  inkzone::renderSimulatorModeLabel("MANUAL");
}

void handleButtons(unsigned long nowMs) {
  const inkzone::ButtonEvent previousEvent =
      previousButton.update(
          digitalRead(kPreviousButtonPin) == LOW,
          nowMs);

  const inkzone::ButtonEvent selectEvent =
      selectButton.update(
          digitalRead(kSelectButtonPin) == LOW,
          nowMs);

  const inkzone::ButtonEvent nextEvent =
      nextButton.update(
          digitalRead(kNextButtonPin) == LOW,
          nowMs);

  if (previousEvent == inkzone::ButtonEvent::kShortPress) {
    viewModeController.previous(
        kManualViewCount,
        nowMs);

    Serial.println("Button: previous");
    renderManualView();
  }

  if (nextEvent == inkzone::ButtonEvent::kShortPress) {
    viewModeController.next(
        kManualViewCount,
        nowMs);

    Serial.println("Button: next");
    renderManualView();
  }

  if (selectEvent == inkzone::ButtonEvent::kShortPress) {
    viewModeController.enterAutomaticMode();
    Serial.println("Button: automatic mode");
  }

  viewModeController.update(nowMs);
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
} 

void printNcaaFootballScoreboard() {
  Serial.println("Requesting NCAA football scoreboard...");

  const inkzone::ProviderResponse response =
      inkzone::fetchEspnNcaaFootballScoreboard();

  Serial.printf(
      "NCAA football result: %s\n",
      inkzone::providerResultName(response.result));

  Serial.printf(
      "NCAA football diagnostic: %s\n",
      response.diagnostic.c_str());

  for (const inkzone::Game& game : response.games) {
    Serial.printf(
        "NCAA football game: %s %d at %s %d\n",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);
  }
}

void printNbaScoreboard() {
  Serial.println("Requesting NBA scoreboard...");

  const inkzone::ProviderResponse response =
      inkzone::fetchEspnNbaScoreboard();

  Serial.printf(
      "NBA result: %s\n",
      inkzone::providerResultName(response.result));

  Serial.printf(
      "NBA diagnostic: %s\n",
      response.diagnostic.c_str());

  for (const inkzone::Game& game : response.games) {
    Serial.printf(
        "NBA game: %s %d at %s %d\n",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);
  }
}

void printNcaaBasketballScoreboard() {
  Serial.println("Requesting NCAA basketball scoreboard...");

  const inkzone::ProviderResponse response =
      inkzone::fetchEspnNcaaBasketballScoreboard();

  Serial.printf(
      "NCAA basketball result: %s\n",
      inkzone::providerResultName(response.result));

  Serial.printf(
      "NCAA basketball diagnostic: %s\n",
      response.diagnostic.c_str());

  for (const inkzone::Game& game : response.games) {
    Serial.printf(
        "NCAA basketball game: %s %d at %s %d\n",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);
  }
}

void printNhlScoreboard() {
  Serial.println("Requesting NHL scoreboard...");

  const inkzone::ProviderResponse response =
      inkzone::fetchEspnNhlScoreboard();

  Serial.printf(
      "NHL result: %s\n",
      inkzone::providerResultName(response.result));

  Serial.printf(
      "NHL diagnostic: %s\n",
      response.diagnostic.c_str());

  for (const inkzone::Game& game : response.games) {
    Serial.printf(
        "NHL game: %s %d at %s %d\n",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);
  }
}

void setup() {
  Serial.begin(kSerialBaudRate);
  delay(500);
  pinMode(kPreviousButtonPin, INPUT_PULLUP);
pinMode(kSelectButtonPin, INPUT_PULLUP);
pinMode(kNextButtonPin, INPUT_PULLUP);

  Serial.println();
  Serial.println("InkZone starting...");
  inkzone::initializeSimulatorDisplay();
  
  const bool settingsLoaded = inkzone::loadSettings(settings);

Serial.printf("Saved settings: %s\n",
              settingsLoaded ? "loaded" : "not found");
  connectToConfiguredWifi();
  printLiveNflScoreboard();
  lastNflUpdateMs = millis();
  selectStateFromSettings();
  printDeviceState();
}

void loop() {
  settingsWebServer.handleClient();
  const unsigned long nowMs = millis();
  handleButtons(nowMs);
  const inkzone::WebsiteViewCommand websiteCommand =
    settingsWebServer.takeViewCommand();

if (websiteCommand ==
    inkzone::WebsiteViewCommand::kPrevious) {
  viewModeController.previous(
      kManualViewCount,
      nowMs);

  renderManualView();
  Serial.println("Website: previous view");
}

if (websiteCommand ==
    inkzone::WebsiteViewCommand::kNext) {
  viewModeController.next(
      kManualViewCount,
      nowMs);

  renderManualView();
  Serial.println("Website: next view");
}

if (websiteCommand ==
    inkzone::WebsiteViewCommand::kAutomatic) {
  viewModeController.enterAutomaticMode();

  if (!cachedNflResponse.games.empty()) {
    const std::string* favoriteTeamIds =
        settings.favorite_team_ids.empty()
            ? nullptr
            : settings.favorite_team_ids.data();

    inkzone::renderSimulatorScoreboard(
        cachedNflResponse,
        favoriteTeamIds,
        settings.favorite_team_ids.size());
        inkzone::renderSimulatorModeLabel("AUTO");
  }

  Serial.println("Website: automatic view");
}
  unsigned long nflRefreshIntervalMs =
    inkzone::refreshIntervalMs(currentActivity);

if (currentActivity ==
    inkzone::ActivityLevel::kLiveFavoriteGame) {
  nflRefreshIntervalMs =
      settings.live_refresh_seconds * 1000UL;
}

if (nowMs - lastNflUpdateMs >= nflRefreshIntervalMs) {
  lastNflUpdateMs = nowMs;
  printLiveNflScoreboard();
}

  if (nowMs - lastHeartbeatMs >= kHeartbeatIntervalMs) {
    lastHeartbeatMs = nowMs;
    Serial.printf("InkZone uptime: %lu seconds, state: %s\n", nowMs / 1000,
                  inkzone::deviceStateName(deviceStatus.state()));
  }

  delay(10);
}
