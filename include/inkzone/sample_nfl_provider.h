#pragma once

#include "inkzone/sports_data_provider.h"

namespace inkzone {

class SampleNflProvider : public SportsDataProvider {
 public:
  const char* name() const override;

  ProviderResponse fetchGames(League league,
                              int64_t start_unix,
                              int64_t end_unix) override;
};

}  // namespace inkzone