#pragma once

#include <stdint.h>

namespace inkzone {

enum class DataError {
  kNone,
  kWifiUnavailable,
  kRequestFailed,
  kInvalidResponse,
  kRateLimited,
};

class DataHealth {
 public:
  void recordSuccess(int64_t updated_at_unix);
  void recordFailure(DataError error);

  bool hasValidData() const;
  int64_t lastUpdatedUnix() const;
  DataError lastError() const;
  uint8_t consecutiveFailures() const;
  uint32_t retryIntervalMs() const;

 private:
  bool has_valid_data_ = false;
  int64_t last_updated_unix_ = 0;
  DataError last_error_ = DataError::kNone;
  uint8_t consecutive_failures_ = 0;
};

const char* dataErrorName(DataError error);

}  // namespace inkzone
