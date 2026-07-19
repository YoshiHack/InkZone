#include "inkzone/data_cache.h"

#include <utility>

namespace inkzone {
namespace {

bool hasValidGameIdentity(const Game& game) {
  return !game.id.empty() && !game.home_team.id.empty() &&
         !game.away_team.id.empty();
}

bool canAnnounceScore(GameStatus status) {
  return status == GameStatus::kLive ||
         status == GameStatus::kHalftime;
}

}

std::vector<ScoreChange>
SportsDataCache::findScoreChanges(
    const SportsDataSnapshot& incoming) const {
  std::vector<ScoreChange> changes;

  if (!has_data_) {
    return changes;
  }

  for (const Game& incomingGame : incoming.games) {
    if (!canAnnounceScore(incomingGame.status)) {
      continue;
    }

    const Game* previousGame = nullptr;

    for (const Game& cachedGame : latest_.games) {
      if (cachedGame.id == incomingGame.id) {
        previousGame = &cachedGame;
        break;
      }
    }

    if (previousGame == nullptr) {
      continue;
    }

    if (incomingGame.home_score >
        previousGame->home_score) {
      changes.push_back({
          incomingGame.id,
          incomingGame.league,
          incomingGame.home_team,
          previousGame->home_score,
          incomingGame.home_score,
      });
    }

    if (incomingGame.away_score >
        previousGame->away_score) {
      changes.push_back({
          incomingGame.id,
          incomingGame.league,
          incomingGame.away_team,
          previousGame->away_score,
          incomingGame.away_score,
      });
    }
  }

  return changes;
}

bool SportsDataCache::storeIfValid(SportsDataSnapshot snapshot) {
  if (snapshot.provider_name.empty() || snapshot.updated_at_unix <= 0) {
    return false;
  }

  for (const Game& game : snapshot.games) {
    if (!hasValidGameIdentity(game)) {
      return false;
    }
  }

  for (const Standing& standing : snapshot.standings) {
    if (standing.team.id.empty()) {
      return false;
    }
  }

  latest_ = std::move(snapshot);
  has_data_ = true;
  return true;
}

bool SportsDataCache::hasData() const {
  return has_data_;
}

const SportsDataSnapshot& SportsDataCache::latest() const {
  return latest_;
}

} 
