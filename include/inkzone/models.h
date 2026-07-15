#pragma once

#include <stdint.h>

#include <string>

namespace inkzone {

enum class League {
  kUnknown,
  kNfl,
  kNcaaFootball,
  kNhl,
  kNba,
  kNcaaBasketball,
  kMls,
  kSoccer,
};

enum class GameStatus {
  kScheduled,
  kLive,
  kHalftime,
  kDelayed,
  kFinal,
  kPostponed,
  kCanceled,
};

struct Team {
  std::string id;
  std::string name;
  std::string abbreviation;
  uint32_t primary_color_rgb = 0;
  uint32_t secondary_color_rgb = 0;
  std::string logo_reference;
};

struct Game {
  std::string id;
  League league = League::kUnknown;
  Team home_team;
  Team away_team;
  int home_score = 0;
  int away_score = 0;
  int64_t scheduled_start_unix = 0;
  GameStatus status = GameStatus::kScheduled;
  std::string period;
  std::string clock;
  std::string venue;
  std::string broadcast_network;
};

struct Standing {
  Team team;
  int wins = 0;
  int losses = 0;
  int ties = 0;
  int conference_rank = 0;
  int division_rank = 0;
};

}  // namespace inkzone

