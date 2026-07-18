#include "inkzone/refresh_scheduler.h"

namespace inkzone {
namespace {

constexpr uint32_t secondMs = 1000;
constexpr uint32_t minuteMs = 60 * secondMs;
constexpr uint32_t hourMs = 60 * minuteMs;

}

uint32_t refreshIntervalMs(ActivityLevel activity, uint8_t retry_attempt) {
  switch (activity) {
    case ActivityLevel::kLiveFavoriteGame:
      return 60 * secondMs;

    case ActivityLevel::kOtherLiveGame:
      return 2 * minuteMs;

    case ActivityLevel::kGameWithinHour:
      return 5 * minuteMs;

    case ActivityLevel::kGamesToday:
      return 15 * minuteMs;

    case ActivityLevel::kNoGamesToday:
      return 2 * hourMs;

    case ActivityLevel::kNetworkRetry: {
      uint8_t attempt = retry_attempt;

      if (attempt > 6) {
        attempt = 6;
      }

      uint32_t interval =
          (1UL << attempt) * 5 * secondMs;

      if (interval > 5 * minuteMs) {
        interval = 5 * minuteMs;
      }

      return interval;
    }
  }

  return 15 * minuteMs;
}

}