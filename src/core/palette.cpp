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
    {InkColor::kBlue, 0, 0, 255},
};

uint32_t colorDistance(uint8_t red, uint8_t green, uint8_t blue,
                       const PaletteEntry& candidate) {
  const int32_t red_delta = static_cast<int32_t>(red) - candidate.red;
  const int32_t green_delta = static_cast<int32_t>(green) - candidate.green;
  const int32_t blue_delta = static_cast<int32_t>(blue) - candidate.blue;

  return 3UL * red_delta * red_delta + 4UL * green_delta * green_delta +
         2UL * blue_delta * blue_delta;
}

}  // namespace

InkColor closestInkColor(uint32_t rgb) {
  const uint8_t red = static_cast<uint8_t>((rgb >> 16) & 0xFF);
  const uint8_t green = static_cast<uint8_t>((rgb >> 8) & 0xFF);
  const uint8_t blue = static_cast<uint8_t>(rgb & 0xFF);

  InkColor closest = kPalette[0].color;
  uint32_t closest_distance = colorDistance(red, green, blue, kPalette[0]);

  for (size_t index = 1; index < sizeof(kPalette) / sizeof(kPalette[0]);
       ++index) {
    const uint32_t distance =
        colorDistance(red, green, blue, kPalette[index]);
    if (distance < closest_distance) {
      closest = kPalette[index].color;
      closest_distance = distance;
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

}  // namespace inkzone
