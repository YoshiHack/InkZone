#include "inkzone/data_cache.h"

#include <utility>

namespace inkzone {
namespace {

bool hasValidGameIdentity(const Game& game) {
  return !game.id.empty() && !game.home_team.id.empty() &&
         !game.away_team.id.empty();
}

}  // namespace

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

}  // namespace inkzone
