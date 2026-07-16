#include "inkzone/settings_storage.h"

#include <Preferences.h>

namespace inkzone {
namespace {

constexpr const char* kStorageNamespace = "inkzone";

}  // namespace

bool saveSettings(const Settings& settings) {
  if (settings.favorite_leagues.empty() ||
      settings.favorite_team_ids.empty()) {
    return false;
  }

  Preferences preferences;

  if (!preferences.begin(kStorageNamespace, false)) {
    return false;
  }

  preferences.putString("timezone", settings.timezone.c_str());
  preferences.putUChar(
      "league",
      static_cast<unsigned char>(settings.favorite_leagues.front()));
  preferences.putString("team",
                        settings.favorite_team_ids.front().c_str());
  preferences.putUInt("live_refresh",
                      settings.live_refresh_seconds);
  preferences.putUInt("idle_refresh",
                      settings.idle_refresh_minutes);
  preferences.putBool("contrast", settings.high_contrast);
  preferences.putBool("configured", true);

  preferences.end();
  return true;
}

bool loadSettings(Settings& settings) {
  Preferences preferences;

  if (!preferences.begin(kStorageNamespace, true)) {
    return false;
  }

  if (!preferences.getBool("configured", false)) {
    preferences.end();
    return false;
  }

  const String timezone =
      preferences.getString("timezone", "UTC");
  const String team =
      preferences.getString("team", "");

  settings.timezone = timezone.c_str();

  settings.favorite_leagues.clear();
  settings.favorite_leagues.push_back(
      static_cast<League>(
          preferences.getUChar(
              "league",
              static_cast<unsigned char>(League::kNfl))));

  settings.favorite_team_ids.clear();

  if (!team.isEmpty()) {
    settings.favorite_team_ids.push_back(team.c_str());
  }

  settings.live_refresh_seconds =
      preferences.getUInt("live_refresh", 60);
  settings.idle_refresh_minutes =
      preferences.getUInt("idle_refresh", 120);
  settings.high_contrast =
      preferences.getBool("contrast", true);

  preferences.end();
  return true;
}

void clearSavedSettings() {
  Preferences preferences;

  if (preferences.begin(kStorageNamespace, false)) {
    preferences.clear();
    preferences.end();
  }
}

}  // namespace inkzone