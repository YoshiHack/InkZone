#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "inkzone/models.h"

namespace inkzone {

enum class ProviderResult {
  kSuccess,
  kNetworkUnavailable,
  kRequestFailed,
  kInvalidResponse,
  kRateLimited,
};

struct ProviderResponse {
  ProviderResult result = ProviderResult::kRequestFailed;
  std::vector<Game> games;
  int64_t fetched_at_unix = 0;
  std::string diagnostic;
};

class SportsDataProvider {
 public:
  virtual ~SportsDataProvider() = default;

  virtual const char* name() const = 0;
  virtual ProviderResponse fetchGames(League league,
                                      int64_t start_unix,
                                      int64_t end_unix) = 0;
};

const char* providerResultName(ProviderResult result);

}  // namespace inkzone