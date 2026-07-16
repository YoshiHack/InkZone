#include "inkzone/refresh_planner.h"

namespace inkzone {
namespace {

constexpr int64_t kHourSeconds = 60 * 60;
constexpr int64_t kRecentGameSeconds = 18 * kHourSeconds;
constexpr int64_t kUpcomingDaySeconds = 24 * kHourSeconds;

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

RefreshPlan makePlan(ActivityLevel activity, uint8_t retry_attempt = 0) {
  return {activity, refreshIntervalMs(activity, retry_attempt)};
}

}  // namespace

RefreshPlan planRefresh(const Game* games, size_t game_count,
                        const std::string* favorite_team_ids,
                        size_t favorite_count, int64_t now_unix,
                        bool network_available, uint8_t retry_attempt) {
  if (!network_available) {
    return makePlan(ActivityLevel::kNetworkRetry, retry_attempt);
  }

  if (games == nullptr || game_count == 0) {
    return makePlan(ActivityLevel::kNoGamesToday);
  }

  for (size_t index = 0; index < game_count; ++index) {
    const Game& game = games[index];
    if (isLive(game.status) &&
        includesFavorite(game, favorite_team_ids, favorite_count)) {
      return makePlan(ActivityLevel::kLiveFavoriteGame);
    }
  }

  for (size_t index = 0; index < game_count; ++index) {
    if (isLive(games[index].status)) {
      return makePlan(ActivityLevel::kOtherLiveGame);
    }
  }

  bool has_game_today = false;
  for (size_t index = 0; index < game_count; ++index) {
    const Game& game = games[index];
    const int64_t seconds_until_start = game.scheduled_start_unix - now_unix;

    if (game.status == GameStatus::kScheduled && seconds_until_start >= 0 &&
        seconds_until_start <= kHourSeconds) {
      return makePlan(ActivityLevel::kGameWithinHour);
    }

    if (seconds_until_start >= -kRecentGameSeconds &&
        seconds_until_start <= kUpcomingDaySeconds) {
      has_game_today = true;
    }
  }

  return makePlan(has_game_today ? ActivityLevel::kGamesToday
                                 : ActivityLevel::kNoGamesToday);
}

const char* activityLevelName(ActivityLevel activity) {
  switch (activity) {
    case ActivityLevel::kLiveFavoriteGame:
      return "favorite game live";
    case ActivityLevel::kOtherLiveGame:
      return "league game live";
    case ActivityLevel::kGameWithinHour:
      return "game within hour";
    case ActivityLevel::kGamesToday:
      return "games today";
    case ActivityLevel::kNoGamesToday:
      return "no games today";
    case ActivityLevel::kNetworkRetry:
      return "network retry";
  }

  return "unknown";
}

}  // namespace inkzone
