#pragma once

#include <stdint.h>

namespace inkzone
{

enum class InkColor
{
    kBlack,
    kWhite,
    kRed,
    kYellow,
    kGreen,
    kBlue
};

InkColor closestInkColor(uint32_t rgb);
InkColor contrastingTextColor(InkColor background);
const char* inkColorName(InkColor color);

}