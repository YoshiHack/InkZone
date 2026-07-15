#include "inkzone/screen_selector.h"

namespace inkzone {
namespace {

constexpr int64_t kPlayingSoonSeconds = 60 * 60;
constexpr int64_t kEndOfTodayWindowSeconds = 24 * 60 * 60;

bool isLive(GameStatus status) {
  return status == GameStatus::kLive || status == GameStatus::kHalftime ||
         status == GameStatus::kDelayed;
}

bool isFavoriteTeam(const std::string& team_id,
                    const std::string* favorite_team_ids,
                    size_t favorite_count) {
  for (size_t index = 0; index < favorite_count; ++index) {
    if (team_id == favorite_team_ids[index]) {
      return true;
    }
  }
  return false;
}

bool includesFavorite(const Game& game,
                      const std::string* favorite_team_ids,
                      size_t favorite_count) {
  return isFavoriteTeam(game.home_team.id, favorite_team_ids, favorite_count) ||
         isFavoriteTeam(game.away_team.id, favorite_team_ids, favorite_count);
}

const Game* findFirstMatching(const Game* games, size_t game_count,
                              const std::string* favorite_team_ids,
                              size_t favorite_count, bool require_favorite,
                              GameStatus required_status) {
  for (size_t index = 0; index < game_count; ++index) {
    const Game& game = games[index];
    const bool status_matches = required_status == GameStatus::kLive
                                    ? isLive(game.status)
                                    : game.status == required_status;
    if (status_matches &&
        (!require_favorite ||
         includesFavorite(game, favorite_team_ids, favorite_count))) {
      return &game;
    }
  }
  return nullptr;
}

}  // namespace

ScreenDecision chooseAutomaticScreen(const Game* games, size_t game_count,
                                     const std::string* favorite_team_ids,
                                     size_t favorite_count,
                                     int64_t now_unix) {
  if (games == nullptr || game_count == 0) {
    return {};
  }

  if (const Game* game = findFirstMatching(
          games, game_count, favorite_team_ids, favorite_count, true,
          GameStatus::kLive)) {
    return {ScreenType::kLiveFavorite, game};
  }

  if (const Game* game = findFirstMatching(
          games, game_count, favorite_team_ids, favorite_count, false,
          GameStatus::kLive)) {
    return {ScreenType::kLiveLeague, game};
  }

  for (size_t index = 0; index < game_count; ++index) {
    const Game& game = games[index];
    const int64_t seconds_until_start = game.scheduled_start_unix - now_unix;
    if (game.status == GameStatus::kScheduled && seconds_until_start >= 0 &&
        seconds_until_start <= kPlayingSoonSeconds &&
        includesFavorite(game, favorite_team_ids, favorite_count)) {
      return {ScreenType::kUpcomingFavorite, &game};
    }
  }

  for (size_t index = 0; index < game_count; ++index) {
    const Game& game = games[index];
    const int64_t seconds_until_start = game.scheduled_start_unix - now_unix;
    if (seconds_until_start >= 0 &&
        seconds_until_start <= kEndOfTodayWindowSeconds &&
        includesFavorite(game, favorite_team_ids, favorite_count)) {
      return {ScreenType::kFavoriteSchedule, &game};
    }
  }

  return {ScreenType::kLeagueScoreboard, nullptr};
}

const char* screenTypeName(ScreenType type) {
  switch (type) {
    case ScreenType::kLiveFavorite:
      return "live favorite";
    case ScreenType::kLiveLeague:
      return "live league";
    case ScreenType::kUpcomingFavorite:
      return "upcoming favorite";
    case ScreenType::kFavoriteSchedule:
      return "favorite schedule";
    case ScreenType::kLeagueScoreboard:
      return "league scoreboard";
    case ScreenType::kIdleDashboard:
      return "idle dashboard";
  }
  return "unknown";
}

}  // namespace inkzone

