#pragma once

#include "inkzone/sports_data_provider.h"

namespace inkzone {

ProviderResponse parseEspnNflScoreboard(const char* json);

}  // namespace inkzone