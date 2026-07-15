#pragma once

#include <stdint.h>

namespace inkzone {

enum class ActivityLevel {
  kLiveFavoriteGame,
  kGameWithinHour,
  kGamesToday,
  kNoGamesToday,
  kNetworkRetry,
};

uint32_t refreshIntervalMs(ActivityLevel activity, uint8_t retry_attempt = 0);

}  // namespace inkzone

