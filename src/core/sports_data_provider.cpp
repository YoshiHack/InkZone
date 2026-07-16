#include "inkzone/sports_data_provider.h"

namespace inkzone {

const char* providerResultName(ProviderResult result) {
  switch (result) {
    case ProviderResult::kSuccess:
      return "success";

    case ProviderResult::kNetworkUnavailable:
      return "network unavailable";

    case ProviderResult::kRequestFailed:
      return "request failed";

    case ProviderResult::kInvalidResponse:
      return "invalid response";

    case ProviderResult::kRateLimited:
      return "rate limited";
  }

  return "unknown";
}

}  // namespace inkzone