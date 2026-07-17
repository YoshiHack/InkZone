#pragma once

#include "inkzone/sports_data_provider.h"

namespace inkzone {

void initializeEpaperDisplay();

void renderEpaperScoreboard(
    const ProviderResponse& response);

}  // namespace inkzone