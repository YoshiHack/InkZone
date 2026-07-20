#include "inkzone/screen_selector.h"

namespace inkzone {
namespace {

constexpr int64_t playingSoonSeconds = 60 * 60;
constexpr int64_t todayWindowSeconds = 24 * 60 * 60;

bool isLive(GameStatus status) {
  return status == GameStatus::kLive ||
         status == GameStatus::kHalftime ||
         status == GameStatus::kDelayed;
}

bool isFavoriteTeam(const Team& team,
                    const std::string* favorite_team_ids,
                    size_t favorite_count) {
  if (favorite_team_ids == nullptr) {
    return false;
  }

  for (size_t i = 0; i < favorite_count; i++) {
    const std::string& favorite =
        favorite_team_ids[i];

    if (team.id == favorite ||
        team.abbreviation == favorite) {
      return true;
    }
  }

  return false;
}

bool includesFavorite(const Game& game,
                      const std::string* favorite_team_ids,
                      size_t favorite_count) {
  return isFavoriteTeam(
             game.home_team,
             favorite_team_ids,
             favorite_count) ||
         isFavoriteTeam(
             game.away_team,
             favorite_team_ids,
             favorite_count);
}

const Game* findFirstMatching(
    const Game* games,
    size_t game_count,
    const std::string* favorite_team_ids,
    size_t favorite_count,
    bool require_favorite,
    GameStatus required_status) {

  for (size_t i = 0; i < game_count; i++) {
    const Game& game = games[i];

    bool statusMatches;

    if (required_status == GameStatus::kLive) {
      statusMatches = isLive(game.status);
    } else {
      statusMatches =
          game.status == required_status;
    }

    if (!statusMatches) {
      continue;
    }

    if (require_favorite &&
        !includesFavorite(
            game,
            favorite_team_ids,
            favorite_count)) {
      continue;
    }

    return &game;
  }

  return nullptr;
}

}

ScreenDecision chooseAutomaticScreen(
    const Game* games,
    size_t game_count,
    const std::string* favorite_team_ids,
    size_t favorite_count,
    int64_t now_unix) {

  if (games == nullptr || game_count == 0) {
    return {};
  }

  const Game* game = findFirstMatching(
      games,
      game_count,
      favorite_team_ids,
      favorite_count,
      true,
      GameStatus::kLive);

  if (game != nullptr) {
    return {ScreenType::kLiveFavorite, game};
  }

  game = findFirstMatching(
      games,
      game_count,
      favorite_team_ids,
      favorite_count,
      false,
      GameStatus::kLive);

  if (game != nullptr) {
    return {ScreenType::kLiveLeague, game};
  }

  for (size_t i = 0; i < game_count; i++) {
    const Game& currentGame = games[i];

    int64_t secondsUntilStart =
        currentGame.scheduled_start_unix - now_unix;

    if (currentGame.status == GameStatus::kScheduled &&
        secondsUntilStart >= 0 &&
        secondsUntilStart <= playingSoonSeconds &&
        includesFavorite(
            currentGame,
            favorite_team_ids,
            favorite_count)) {
      return {
          ScreenType::kUpcomingFavorite,
          &currentGame
      };
    }
  }

  for (size_t i = 0; i < game_count; i++) {
    const Game& currentGame = games[i];

    int64_t secondsUntilStart =
        currentGame.scheduled_start_unix - now_unix;

    if (secondsUntilStart >= 0 &&
        secondsUntilStart <= todayWindowSeconds &&
        includesFavorite(
            currentGame,
            favorite_team_ids,
            favorite_count)) {
      return {
          ScreenType::kFavoriteSchedule,
          &currentGame
      };
    }
  }

  for (size_t i = 0; i < game_count; i++) {
    const Game& currentGame = games[i];

    int64_t secondsUntilStart =
        currentGame.scheduled_start_unix - now_unix;

    if (currentGame.status == GameStatus::kScheduled &&
        secondsUntilStart >= 0 &&
        secondsUntilStart <= todayWindowSeconds) {
      return {
          ScreenType::kLeagueScoreboard,
          nullptr
      };
    }
  }

  return {
      ScreenType::kIdleDashboard,
      nullptr
  };
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

}