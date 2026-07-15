#include "inkzone/refresh_scheduler.h"

namespace inkzone {
namespace {

constexpr uint32_t kSecondMs = 1000UL;
constexpr uint32_t kMinuteMs = 60UL * kSecondMs;
constexpr uint32_t kHourMs = 60UL * kMinuteMs;

}  // namespace

uint32_t refreshIntervalMs(ActivityLevel activity, uint8_t retry_attempt) {
  switch (activity) {
    case ActivityLevel::kLiveFavoriteGame:
      return 60UL * kSecondMs;
    case ActivityLevel::kGameWithinHour:
      return 5UL * kMinuteMs;
    case ActivityLevel::kGamesToday:
      return 15UL * kMinuteMs;
    case ActivityLevel::kNoGamesToday:
      return 2UL * kHourMs;
    case ActivityLevel::kNetworkRetry: {
      const uint8_t capped_attempt = retry_attempt > 6 ? 6 : retry_attempt;
      const uint32_t interval = (1UL << capped_attempt) * 5UL * kSecondMs;
      return interval > 5UL * kMinuteMs ? 5UL * kMinuteMs : interval;
    }
  }
  return 15UL * kMinuteMs;
}

}  // namespace inkzone

