#include "inkzone/simulator_display.h"

#include <Adafruit_ILI9341.h>

namespace inkzone {
namespace {

constexpr int kDisplayCsPin = 15;
constexpr int kDisplayDcPin = 2;

Adafruit_ILI9341 display(
    kDisplayCsPin,
    kDisplayDcPin);

}  // namespace

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

void renderSimulatorScoreboard(
    const ProviderResponse& response) {
  display.fillScreen(ILI9341_BLACK);

  display.fillRect(
      0,
      0,
      display.width(),
      40,
      ILI9341_BLUE);

  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(3);
  display.setCursor(10, 8);
  display.print("NFL SCOREBOARD");

  int rowY = 50;
  size_t gamesDrawn = 0;

  for (const Game& game : response.games) {
    if (gamesDrawn >= 5) {
      break;
    }

    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE);
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

// Forward declaration so statusLabel can be used above.
const char* statusLabel(GameStatus status);

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

}  // namespace inkzone