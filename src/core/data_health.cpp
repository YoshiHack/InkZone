#include "inkzone/data_health.h"

#include "inkzone/refresh_scheduler.h"

namespace inkzone {

void DataHealth::recordSuccess(int64_t updated_at_unix) {
  if (updated_at_unix <= 0) {
    return;
  }

  has_valid_data_ = true;
  last_updated_unix_ = updated_at_unix;
  last_error_ = DataError::kNone;
  consecutive_failures_ = 0;
}

void DataHealth::recordFailure(DataError error) {
  if (error == DataError::kNone) {
    return;
  }

  last_error_ = error;
  if (consecutive_failures_ < 255) {
    ++consecutive_failures_;
  }
}

bool DataHealth::hasValidData() const {
  return has_valid_data_;
}

int64_t DataHealth::lastUpdatedUnix() const {
  return last_updated_unix_;
}

DataError DataHealth::lastError() const {
  return last_error_;
}

uint8_t DataHealth::consecutiveFailures() const {
  return consecutive_failures_;
}

uint32_t DataHealth::retryIntervalMs() const {
  if (consecutive_failures_ == 0) {
    return 0;
  }

  return refreshIntervalMs(ActivityLevel::kNetworkRetry,
                           consecutive_failures_ - 1);
}

const char* dataErrorName(DataError error) {
  switch (error) {
    case DataError::kNone:
      return "none";
    case DataError::kWifiUnavailable:
      return "Wi-Fi unavailable";
    case DataError::kRequestFailed:
      return "sports request failed";
    case DataError::kInvalidResponse:
      return "invalid sports response";
    case DataError::kRateLimited:
      return "sports provider rate limited";
  }

  return "unknown";
}

}  // namespace inkzone
