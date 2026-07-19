#include "inkzone/simulator_display.h"

#include <time.h>
#include <Adafruit_ILI9341.h>

namespace inkzone {
namespace {

constexpr int kDisplayCsPin = 15;
constexpr int kDisplayDcPin = 2;

Adafruit_ILI9341 display(
    kDisplayCsPin,
    kDisplayDcPin);

bool isFavoriteTeam(
    const Team& team,
    const std::string* favoriteTeamIds,
    size_t favoriteCount) {
  for (size_t index = 0; index < favoriteCount; ++index) {
    const std::string& favorite = favoriteTeamIds[index];

    if (team.id == favorite ||
        team.abbreviation == favorite) {
      return true;
    }
  }

  return false;
}

uint16_t rgbToDisplayColor(
    uint32_t rgb,
    uint16_t fallbackColor) {
  if (rgb == 0) {
    return fallbackColor;
  }

  const uint8_t red =
      static_cast<uint8_t>((rgb >> 16) & 0xFF);

  const uint8_t green =
      static_cast<uint8_t>((rgb >> 8) & 0xFF);

  const uint8_t blue =
      static_cast<uint8_t>(rgb & 0xFF);

  return static_cast<uint16_t>(
      ((red & 0xF8) << 8) |
      ((green & 0xFC) << 3) |
      (blue >> 3));
}


}  

const char* leagueLabel(League league) {
  switch (league) {
    case League::kNfl:
      return "NFL";
    case League::kNcaaFootball:
      return "NCAA FOOTBALL";
    case League::kNba:
      return "NBA";
    case League::kNcaaBasketball:
      return "NCAA BASKETBALL";
    case League::kNhl:
      return "NHL";
    default:
      return "SCORES";
  }
}

const char* statusLabel(GameStatus status);

void initializeSimulatorDisplay() {
  display.begin();
  display.setRotation(1);
  display.setTextWrap(false);

  display.fillScreen(ILI9341_BLACK);
  display.fillRect(
      0,
      0,
      display.width(),
      48,
      ILI9341_BLUE);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(3);
  display.setCursor(12, 12);
  display.print("INKZONE");

  display.setTextSize(2);
  display.setCursor(12, 84);
  display.print("Starting...");
}

void renderSimulatorModeLabel(
    const char* label) {
  display.fillRoundRect(
      display.width() - 62,
      4,
      58,
      30,
      4,
      ILI9341_NAVY);

  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(1);
  display.setCursor(
      display.width() - 56,
      15);
  display.print(label);
}

void renderSimulatorScoreboard(
    const ProviderResponse& response,
    const std::string* favoriteTeamIds,
    size_t favoriteCount) {
  display.fillScreen(ILI9341_BLACK);

  void renderSimulatorUpcomingGame(
    const Game& game);

  display.fillRect(
      0,
      0,
      display.width(),
      40,
      ILI9341_BLUE);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(3);
  display.setCursor(10, 8);
  const League league =
    response.games.empty()
        ? League::kUnknown
        : response.games.front().league;

display.print(leagueLabel(league));
display.print(" SCOREBOARD");

  int rowY = 50;
  size_t gamesDrawn = 0;

  for (const Game& game : response.games) {
    if (gamesDrawn >= 5) {
      break;
    }

    const bool favoriteGame =
        isFavoriteTeam(
            game.home_team,
            favoriteTeamIds,
            favoriteCount) ||
        isFavoriteTeam(
            game.away_team,
            favoriteTeamIds,
            favoriteCount);

    if (favoriteGame) {
      display.fillRoundRect(
          6,
          rowY - 4,
          display.width() - 12,
          30,
          4,
          ILI9341_DARKCYAN);
    }
    display.fillRect(
    6,
    rowY - 4,
    4,
    30,
    rgbToDisplayColor(
        game.away_team.primary_color_rgb,
        ILI9341_DARKGREY));

display.fillRect(
    display.width() - 10,
    rowY - 4,
    4,
    30,
    rgbToDisplayColor(
        game.home_team.primary_color_rgb,
        ILI9341_DARKGREY));

    display.setTextSize(2);
    display.setTextColor(
        favoriteGame
            ? ILI9341_YELLOW
            : ILI9341_WHITE);
    display.setCursor(12, rowY);

    display.printf(
        "%s %d   @   %s %d",
        game.away_team.abbreviation.c_str(),
        game.away_score,
        game.home_team.abbreviation.c_str(),
        game.home_score);

        display.setTextSize(1);
display.setTextColor(ILI9341_YELLOW);
display.setCursor(250, rowY + 5);
display.print(statusLabel(game.status));

    display.drawFastHLine(
        8,
        rowY + 25,
        display.width() - 16,
        ILI9341_DARKGREY);

    rowY += 38;
    ++gamesDrawn;
  }

  if (gamesDrawn == 0) {
    display.setTextSize(2);
    display.setCursor(12, 72);
    display.print("No games available");
  }
}

const char* statusLabel(GameStatus status) {
  switch (status) {
    case GameStatus::kScheduled:
      return "UPCOMING";

    case GameStatus::kLive:
      return "LIVE";

    case GameStatus::kHalftime:
      return "HALFTIME";

    case GameStatus::kDelayed:
      return "DELAYED";

    case GameStatus::kFinal:
      return "FINAL";

    case GameStatus::kPostponed:
      return "POSTPONED";

    case GameStatus::kCanceled:
      return "CANCELED";
  }

  return "";
}

void renderSimulatorUpcomingGame(
    const Game& game) {
  display.fillScreen(ILI9341_BLACK);

  display.fillRect(
      0,
      0,
      display.width(),
      40,
      ILI9341_BLUE);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 12);
  display.print("NEXT FAVORITE GAME");

  display.fillRect(
      8,
      58,
      8,
      70,
      rgbToDisplayColor(
          game.away_team.primary_color_rgb,
          ILI9341_DARKGREY));

  display.fillRect(
      display.width() - 16,
      58,
      8,
      70,
      rgbToDisplayColor(
          game.home_team.primary_color_rgb,
          ILI9341_DARKGREY));

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(4);
  display.setCursor(26, 74);
  display.print(game.away_team.abbreviation.c_str());

  display.setTextSize(2);
  display.setCursor(140, 84);
  display.print("@");

  display.setTextSize(4);
  display.setCursor(180, 74);
  display.print(game.home_team.abbreviation.c_str());

  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(2);
  display.setCursor(12, 150);
  display.print(statusLabel(game.status));

  char startTimeText[32] = "Start time unavailable";

  if (game.scheduled_start_unix > 0) {
    const time_t startTime =
        static_cast<time_t>(
            game.scheduled_start_unix);

    struct tm utcTime;

    if (gmtime_r(&startTime, &utcTime) != nullptr) {
      strftime(
          startTimeText,
          sizeof(startTimeText),
          "%b %d, %H:%M UTC",
          &utcTime);
    }
  }

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.setCursor(12, 188);
  display.print(startTimeText);
}

void renderSimulatorIdleDashboard(
    const Game* nextFavoriteGame,
    int64_t nowUnix) {
  display.fillScreen(ILI9341_BLACK);

  display.fillRect(0, 0, display.width(), 40, ILI9341_BLUE);
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 12);
  display.print("INKZONE DASHBOARD");

  char currentTimeText[32] = "Time unavailable";
  if (nowUnix > 0) {
    const time_t currentTime = static_cast<time_t>(nowUnix);
    struct tm utcTime;
    if (gmtime_r(&currentTime, &utcTime) != nullptr) {
      strftime(currentTimeText, sizeof(currentTimeText),
               "%a %b %d  %H:%M UTC", &utcTime);
    }
  }

  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(2);
  display.setCursor(12, 64);
  display.print(currentTimeText);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(2);
  display.setCursor(12, 108);
  display.print("NEXT FAVORITE GAME");

  if (nextFavoriteGame == nullptr) {
    display.setTextColor(ILI9341_LIGHTGREY);
    display.setCursor(12, 146);
    display.print("No favorite team set");
    return;
  }

  display.fillRect(
      8, 166, 8, 45,
      rgbToDisplayColor(nextFavoriteGame->away_team.primary_color_rgb,
                        ILI9341_DARKGREY));

  display.fillRect(
      display.width() - 16, 166, 8, 45,
      rgbToDisplayColor(nextFavoriteGame->home_team.primary_color_rgb,
                        ILI9341_DARKGREY));

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(3);
  display.setCursor(28, 176);
  display.printf("%s @ %s",
                 nextFavoriteGame->away_team.abbreviation.c_str(),
                 nextFavoriteGame->home_team.abbreviation.c_str());

  char startTimeText[32] = "Start time unavailable";
  if (nextFavoriteGame->scheduled_start_unix > 0) {
    const time_t startTime =
        static_cast<time_t>(nextFavoriteGame->scheduled_start_unix);
    struct tm utcStartTime;
    if (gmtime_r(&startTime, &utcStartTime) != nullptr) {
      strftime(startTimeText, sizeof(startTimeText),
               "%b %d, %H:%M UTC", &utcStartTime);
    }
  }

  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(2);
  display.setCursor(12, 216);
  display.print(startTimeText);
}

void renderSimulatorScoreAlert(
    const ScoreChange& scoreChange) {
  const bool isGoal =
      scoreChange.league == League::kNhl ||
      scoreChange.league == League::kSoccer;

  const uint16_t teamColor =
      rgbToDisplayColor(
          scoreChange.scoring_team.primary_color_rgb,
          ILI9341_BLUE);

  const char* alertText =
      isGoal ? "GOAL!" : "SCORE UPDATE";

  for (int flash = 0; flash < 2; ++flash) {
    display.fillScreen(teamColor);
    delay(180);

    display.fillScreen(ILI9341_BLACK);
    delay(120);
  }

  display.fillScreen(teamColor);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(4);
  display.setCursor(18, 42);
  display.print(alertText);

  display.setTextColor(ILI9341_BLACK);
  display.setTextSize(4);
  display.setCursor(34, 108);
  display.print(
      scoreChange.scoring_team.abbreviation.c_str());

  display.setTextSize(3);
  display.setCursor(34, 162);
  display.printf(
      "%d -> %d",
      scoreChange.previous_score,
      scoreChange.updated_score);

  delay(1100);
}

} 