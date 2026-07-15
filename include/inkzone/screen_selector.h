#pragma once

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "inkzone/models.h"

namespace inkzone {

enum class ScreenType {
  kLiveFavorite,
  kLiveLeague,
  kUpcomingFavorite,
  kFavoriteSchedule,
  kLeagueScoreboard,
  kIdleDashboard,
};

struct ScreenDecision {
  ScreenType type = ScreenType::kIdleDashboard;
  const Game* featured_game = nullptr;
};

ScreenDecision chooseAutomaticScreen(const Game* games, size_t game_count,
                                     const std::string* favorite_team_ids,
                                     size_t favorite_count,
                                     int64_t now_unix);

const char* screenTypeName(ScreenType type);

}  // namespace inkzone

