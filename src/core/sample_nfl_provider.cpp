#include "inkzone/sample_nfl_provider.h"

namespace inkzone {

const char* SampleNflProvider::name() const {
  return "Sample NFL Provider";
}

ProviderResponse SampleNflProvider::fetchGames(
    League league,
    int64_t start_unix,
    int64_t end_unix) {

  ProviderResponse response;

  if (league != League::kNfl) {
    response.result =
        ProviderResult::kInvalidResponse;

    response.diagnostic =
        "Sample provider only supports the NFL";

    return response;
  }

  if (start_unix <= 0 ||
      end_unix < start_unix) {
    response.result =
        ProviderResult::kInvalidResponse;

    response.diagnostic =
        "Invalid requested time range";

    return response;
  }

  Game game;

  game.id = "sample-nfl-001";
  game.league = League::kNfl;

  game.away_team.id = "buf";
  game.away_team.name = "Buffalo Bills";
  game.away_team.abbreviation = "BUF";
  game.away_team.primary_color_rgb = 0x00338D;
  game.away_team.secondary_color_rgb = 0xC60C30;

  game.home_team.id = "kc";
  game.home_team.name = "Kansas City Chiefs";
  game.home_team.abbreviation = "KC";
  game.home_team.primary_color_rgb = 0xE31837;
  game.home_team.secondary_color_rgb = 0xFFB81C;

  game.away_score = 17;
  game.home_score = 14;

  game.scheduled_start_unix =
      start_unix + 30 * 60;

  game.status = GameStatus::kLive;
  game.period = "3rd";
  game.clock = "08:42";
  game.venue = "Sample Stadium";
  game.broadcast_network = "Sample Network";

  response.games.push_back(game);

  response.result =
      ProviderResult::kSuccess;

  response.fetched_at_unix =
      start_unix;

  response.diagnostic =
      "Loaded built-in sample data";

  return response;
}

}