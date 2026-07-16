#include "inkzone/data_freshness.h"

namespace inkzone {
namespace {

constexpr int64_t kFreshWindowSeconds = 15 * 60;
constexpr int64_t kAgingWindowSeconds = 60 * 60;

}  // namespace

DataFreshness classifyDataFreshness(bool has_valid_data,
                                    int64_t last_updated_unix,
                                    int64_t now_unix) {
  if (!has_valid_data || last_updated_unix <= 0) {
    return DataFreshness::kUnavailable;
  }

  if (now_unix <= last_updated_unix) {
    return DataFreshness::kFresh;
  }

  const int64_t age_seconds = now_unix - last_updated_unix;
  if (age_seconds <= kFreshWindowSeconds) {
    return DataFreshness::kFresh;
  }

  if (age_seconds <= kAgingWindowSeconds) {
    return DataFreshness::kAging;
  }

  return DataFreshness::kStale;
}

const char* dataFreshnessName(DataFreshness freshness) {
  switch (freshness) {
    case DataFreshness::kUnavailable:
      return "unavailable";
    case DataFreshness::kFresh:
      return "fresh";
    case DataFreshness::kAging:
      return "aging";
    case DataFreshness::kStale:
      return "stale";
  }

  return "unknown";
}

}  // namespace inkzone
