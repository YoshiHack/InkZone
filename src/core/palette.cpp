#include "inkzone/palette.h"

#include <stddef.h>

namespace inkzone {
namespace {

struct PaletteEntry {
  InkColor color;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

constexpr PaletteEntry kPalette[] = {
    {InkColor::kBlack, 0, 0, 0},
    {InkColor::kWhite, 255, 255, 255},
    {InkColor::kRed, 255, 0, 0},
    {InkColor::kYellow, 255, 255, 0},
    {InkColor::kGreen, 0, 166, 81},
    {InkColor::kBlue, 0, 0, 255}
};

uint32_t colorDistance(
    uint8_t red,
    uint8_t green,
    uint8_t blue,
    const PaletteEntry& candidate) {

  int32_t redDelta =
      static_cast<int32_t>(red) - candidate.red;
  int32_t greenDelta =
      static_cast<int32_t>(green) - candidate.green;
  int32_t blueDelta =
      static_cast<int32_t>(blue) - candidate.blue;

  return 3UL * redDelta * redDelta +
         4UL * greenDelta * greenDelta +
         2UL * blueDelta * blueDelta;
}

}

InkColor closestInkColor(uint32_t rgb) {
  uint8_t red = (rgb >> 16) & 0xFF;
  uint8_t green = (rgb >> 8) & 0xFF;
  uint8_t blue = rgb & 0xFF;

  InkColor closest = kPalette[0].color;
  uint32_t bestDistance =
      colorDistance(red, green, blue, kPalette[0]);

  for (size_t i = 1;
       i < sizeof(kPalette) / sizeof(kPalette[0]);
       i++) {

    uint32_t distance =
        colorDistance(red, green, blue, kPalette[i]);

    if (distance < bestDistance) {
      bestDistance = distance;
      closest = kPalette[i].color;
    }
  }

  return closest;
}

InkColor contrastingTextColor(InkColor background) {
  switch (background) {
    case InkColor::kWhite:
    case InkColor::kYellow:
    case InkColor::kGreen:
      return InkColor::kBlack;

    case InkColor::kBlack:
    case InkColor::kRed:
    case InkColor::kBlue:
      return InkColor::kWhite;
  }

  return InkColor::kBlack;
}

const char* inkColorName(InkColor color) {
  switch (color) {
    case InkColor::kBlack:
      return "black";

    case InkColor::kWhite:
      return "white";

    case InkColor::kRed:
      return "red";

    case InkColor::kYellow:
      return "yellow";

    case InkColor::kGreen:
      return "green";

    case InkColor::kBlue:
      return "blue";
  }

  return "unknown";
}

}