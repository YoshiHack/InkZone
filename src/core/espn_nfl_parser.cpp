#include "inkzone/espn_nfl_parser.h"

#include <ArduinoJson.h>

#include <cstdio>
#include <cstdlib>
#include <string>

namespace inkzone {
namespace {

int64_t daysFromCivil(int year, unsigned month, unsigned day) {
  year -= month <= 2;

  const int era = (year >= 0 ? year : year - 399) / 400;
  const unsigned yearOfEra =
      static_cast<unsigned>(year - era * 400);
  const unsigned dayOfYear =
      (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  const unsigned dayOfEra =
      yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;

  return static_cast<int64_t>(era) * 146097 +
         static_cast<int64_t>(dayOfEra) - 719468;
}

int64_t parseStartTime(const char* dateText) {
  if (dateText == nullptr || dateText[0] == '\0') {
    return 0;
  }

  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;

  const int fieldsRead =
      std::sscanf(dateText,
                  "%d-%d-%dT%d:%d:%dZ",
                  &year,
                  &month,
                  &day,
                  &hour,
                  &minute,
                  &second);

  if (fieldsRead != 6) {
    return 0;
  }

  const int64_t days =
      daysFromCivil(year,
                    static_cast<unsigned>(month),
                    static_cast<unsigned>(day));

  return days * 86400 +
         static_cast<int64_t>(hour) * 3600 +
         static_cast<int64_t>(minute) * 60 +
         second;
}

std::string periodName(int period) {
  switch (period) {
    case 1:
      return "1st";
    case 2:
      return "2nd";
    case 3:
      return "3rd";
    case 4:
      return "4th";
    default:
      return period > 0 ? std::to_string(period) : "";
  }
}

GameStatus parseStatus(JsonObjectConst status) {
  const bool completed = status["type"]["completed"] | false;
  const char* state = status["type"]["state"] | "";

  if (completed) {
    return GameStatus::kFinal;
  }

  if (std::string(state) == "in") {
    return GameStatus::kLive;
  }

  return GameStatus::kScheduled;
}

uint32_t parseTeamColor(const char* colorText) {
  if (colorText == nullptr || colorText[0] == '\0') {
    return 0;
  }

  return static_cast<uint32_t>(
      std::strtoul(colorText, nullptr, 16));
}

void readCompetitor(JsonObjectConst competitor, Game& game) {
  Team team;
  team.id = competitor["team"]["id"] | "";
  team.name = competitor["team"]["displayName"] | "";
  team.abbreviation = competitor["team"]["abbreviation"] | "";
  team.primary_color_rgb =
      parseTeamColor(competitor["team"]["color"] | "");
  team.secondary_color_rgb =
      parseTeamColor(competitor["team"]["alternateColor"] | "");

  const char* scoreText = competitor["score"] | "0";
  const int score = std::atoi(scoreText);
  const char* homeAway = competitor["homeAway"] | "";

  if (std::string(homeAway) == "home") {
    game.home_team = team;
    game.home_score = score;
  } else if (std::string(homeAway) == "away") {
    game.away_team = team;
    game.away_score = score;
  }
}

void configureScoreboardFilter(JsonDocument& filter) {
  filter["events"][0]["id"] = true;
  filter["events"][0]["date"] = true;

  filter["events"][0]["status"]["period"] = true;
  filter["events"][0]["status"]["displayClock"] = true;
  filter["events"][0]["status"]["type"]["state"] = true;
  filter["events"][0]["status"]["type"]["completed"] = true;

  filter["events"][0]["competitions"][0]["competitors"][0]["homeAway"] =
      true;
  filter["events"][0]["competitions"][0]["competitors"][0]["score"] =
      true;
  filter["events"][0]["competitions"][0]["competitors"][0]["team"]["id"] =
      true;
  filter["events"][0]["competitions"][0]["competitors"][0]["team"]
        ["displayName"] = true;
  filter["events"][0]["competitions"][0]["competitors"][0]["team"]
        ["abbreviation"] = true;
  filter["events"][0]["competitions"][0]["competitors"][0]["team"]
        ["color"] = true;
  filter["events"][0]["competitions"][0]["competitors"][0]["team"]
        ["alternateColor"] = true;
}

ProviderResponse parseScoreboardDocument(
    const JsonDocument& document,
    League league) {
  ProviderResponse response;

  const JsonArrayConst events =
      document["events"].as<JsonArrayConst>();

  if (events.isNull()) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic = "Scoreboard did not contain events";
    return response;
  }

  for (JsonObjectConst event : events) {
    const JsonArrayConst competitions =
        event["competitions"].as<JsonArrayConst>();

    if (competitions.size() == 0) {
      continue;
    }

    const JsonObjectConst competition = competitions[0];
    const JsonArrayConst competitors =
        competition["competitors"].as<JsonArrayConst>();

    if (competitors.size() < 2) {
      continue;
    }

    Game game;
    game.id = event["id"] | "";
    game.league = league;

    const char* startTime = event["date"] | "";
    game.scheduled_start_unix = parseStartTime(startTime);

    const JsonObjectConst status = event["status"];
    const int period = status["period"] | 0;

    game.status = parseStatus(status);
    game.period = periodName(period);
    game.clock = status["displayClock"] | "";

    for (JsonObjectConst competitor : competitors) {
      readCompetitor(competitor, game);
    }

    if (!game.home_team.abbreviation.empty() &&
        !game.away_team.abbreviation.empty()) {
      response.games.push_back(game);
    }
  }

  if (response.games.empty()) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic =
        "Scoreboard contained no usable games";
    return response;
  }

  response.result = ProviderResult::kSuccess;
  response.diagnostic = "Parsed scoreboard";
  return response;
}

}

ProviderResponse parseEspnScoreboard(
    const char* json,
    League league) {
  ProviderResponse response;

  if (json == nullptr || json[0] == '\0') {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic = "Scoreboard JSON was empty";
    return response;
  }

  JsonDocument filter;
  configureScoreboardFilter(filter);

  JsonDocument document;
  const DeserializationError error =
      deserializeJson(
          document,
          json,
          DeserializationOption::Filter(filter),
          DeserializationOption::NestingLimit(20));

  if (error) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic = error.c_str();
    return response;
  }

  return parseScoreboardDocument(document, league);
}

ProviderResponse parseEspnScoreboard(
    Stream& jsonStream,
    League league) {
  ProviderResponse response;

  JsonDocument filter;
  configureScoreboardFilter(filter);

  JsonDocument document;
  const DeserializationError error =
      deserializeJson(
          document,
          jsonStream,
          DeserializationOption::Filter(filter),
          DeserializationOption::NestingLimit(20));

  if (error) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic = error.c_str();
    return response;
  }

  return parseScoreboardDocument(document, league);
}

ProviderResponse parseEspnNflScoreboard(const char* json) {
  return parseEspnScoreboard(json, League::kNfl);
}

}