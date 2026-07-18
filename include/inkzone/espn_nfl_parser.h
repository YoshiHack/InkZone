#pragma once

#include <Stream.h>

#include "inkzone/sports_data_provider.h"

namespace inkzone {

ProviderResponse parseEspnScoreboard(
    const char* json,
    League league);

    ProviderResponse parseEspnScoreboard(
    Stream& jsonStream,
    League league);

ProviderResponse parseEspnNflScoreboard(const char* json);

}  // namespace inkzone