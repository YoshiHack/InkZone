#pragma once

#include <stdint.h>

namespace inkzone {

enum class DataFreshness {
  kUnavailable,
  kFresh,
  kAging,
  kStale,
};

DataFreshness classifyDataFreshness(bool has_valid_data,
                                    int64_t last_updated_unix,
                                    int64_t now_unix);
const char* dataFreshnessName(DataFreshness freshness);

}  // namespace inkzone
