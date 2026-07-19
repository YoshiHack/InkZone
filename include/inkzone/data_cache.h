#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "inkzone/models.h"

namespace inkzone {

struct SportsDataSnapshot {
  std::string provider_name;
  int64_t updated_at_unix = 0;
  std::vector<Game> games;
  std::vector<Standing> standings;
};

struct ScoreChange {
  std::string game_id;
  League league = League::kUnknown;
  Team scoring_team;
  int previous_score = 0;
  int updated_score = 0;
};

class SportsDataCache {
 public:
  bool storeIfValid(SportsDataSnapshot snapshot);
  bool hasData() const;
  const SportsDataSnapshot& latest() const;
  std::vector<ScoreChange> findScoreChanges(
    const SportsDataSnapshot& incoming) const;

 private:
  bool has_data_ = false;
  SportsDataSnapshot latest_;
};

}
