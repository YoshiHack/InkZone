#pragma once

#include "inkzone/sports_data_provider.h"

namespace inkzone {

ProviderResponse parseEspnScoreboard(
    const char* json,
    League league);

ProviderResponse parseEspnNflScoreboard(const char* json);

}  // namespace inkzone