#include "inkzone/epaper_display.h"

#include <SPI.h>
#include <GxEPD2_BW.h>

namespace inkzone {
namespace {

constexpr int kDisplayCsPin = 15;
constexpr int kDisplayDcPin = 27;
constexpr int kDisplayResetPin = 26;
constexpr int kDisplayBusyPin = 25;

GxEPD2_BW<GxEPD2_290_T94_V2,
          GxEPD2_290_T94_V2::HEIGHT> display(
    GxEPD2_290_T94_V2(
        kDisplayCsPin,
        kDisplayDcPin,
        kDisplayResetPin,
        kDisplayBusyPin));

}  // namespace

void initializeEpaperDisplay() {
  pinMode(kDisplayCsPin, OUTPUT);
  pinMode(kDisplayDcPin, OUTPUT);
  pinMode(kDisplayResetPin, OUTPUT);
  pinMode(kDisplayBusyPin, INPUT);

  SPI.begin(18, -1, 23);

  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setTextWrap(false);
Serial.printf(
    "EPD size: %d x %d, pages: %d\n",
    display.width(),
    display.height(),
    display.pages());

display.epd2.clearScreen(0xFF);
}

void renderEpaperScoreboard(
    const ProviderResponse& response) {
  display.setFullWindow();
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);

    display.setTextSize(2);
    display.setCursor(8, 18);
    display.print("INKZONE NFL");

    int rowY = 42;
    size_t gamesDrawn = 0;

    for (const Game& game : response.games) {
      if (gamesDrawn >= 4) {
        break;
      }

      display.setCursor(8, rowY);
      display.printf(
          "%s %d @ %s %d",
          game.away_team.abbreviation.c_str(),
          game.away_score,
          game.home_team.abbreviation.c_str(),
          game.home_score);

      rowY += 22;
      ++gamesDrawn;
    }

    if (gamesDrawn == 0) {
      display.setCursor(8, 52);
      display.print("No games");
    }
  } while (display.nextPage());
}

}  // namespace inkzone