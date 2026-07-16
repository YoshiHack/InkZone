#include "inkzone/settings.h"

namespace inkzone {

SettingsError validateSettings(const Settings& settings) {
  if (settings.timezone.empty()) {
    return SettingsError::kMissingTimezone;
  }

  if (settings.favorite_leagues.empty()) {
    return SettingsError::kNoFavoriteLeagues;
  }

  if (settings.favorite_team_ids.empty()) {
    return SettingsError::kNoFavoriteTeams;
  }

  if (settings.live_refresh_seconds < 30 ||
      settings.live_refresh_seconds > 300) {
    return SettingsError::kLiveRefreshOutOfRange;
  }

  if (settings.idle_refresh_minutes < 30 ||
      settings.idle_refresh_minutes > 360) {
    return SettingsError::kIdleRefreshOutOfRange;
  }

  return SettingsError::kNone;
}

const char* settingsErrorName(SettingsError error) {
  switch (error) {
    case SettingsError::kNone:
      return "none";
    case SettingsError::kMissingTimezone:
      return "missing timezone";
    case SettingsError::kNoFavoriteLeagues:
      return "no favorite leagues";
    case SettingsError::kNoFavoriteTeams:
      return "no favorite teams";
    case SettingsError::kLiveRefreshOutOfRange:
      return "live refresh interval out of range";
    case SettingsError::kIdleRefreshOutOfRange:
      return "idle refresh interval out of range";
  }

  return "unknown";
}

}  // namespace inkzone
