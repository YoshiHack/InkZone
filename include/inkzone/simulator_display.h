#pragma once

#include "inkzone/sports_data_provider.h"

namespace inkzone {

void initializeSimulatorDisplay();

void renderSimulatorScoreboard(
    const ProviderResponse& response);

}  // namespace inkzone