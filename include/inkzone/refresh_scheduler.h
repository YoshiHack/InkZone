#pragma once

#include <stdint.h>

namespace inkzone
{

enum class ActivityLevel
{
    kLiveFavoriteGame,
    kOtherLiveGame,
    kGameWithinHour,
    kGamesToday,
    kNoGamesToday,
    kNetworkRetry
};

uint32_t refreshIntervalMs(
    ActivityLevel activity,
    uint8_t retry_attempt = 0);

}
