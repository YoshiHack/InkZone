#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "inkzone/models.h"

namespace inkzone {

enum class SettingsError {
  kNone,
  kMissingTimezone,
  kNoFavoriteLeagues,
  kNoFavoriteTeams,
  kLiveRefreshOutOfRange,
  kIdleRefreshOutOfRange,
};

struct Settings {
  std::string timezone = "UTC";
  std::string wifi_ssid;
  std::string wifi_password;
  std::vector<League> favorite_leagues;
  std::vector<std::string> favorite_team_ids;
  uint32_t live_refresh_seconds = 60;
  uint32_t idle_refresh_minutes = 120;
  bool high_contrast = true;
};
SettingsError validateSettings(const Settings& settings);
const char* settingsErrorName(SettingsError error);

}  // namespace inkzone
