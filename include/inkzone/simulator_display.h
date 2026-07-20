#pragma once

#include <stddef.h>

#include <string>

#include "inkzone/sports_data_provider.h"
#include "inkzone/data_cache.h"

namespace inkzone {

void initializeSimulatorDisplay();

void renderSimulatorModeLabel(
    const char* label);

void renderSimulatorScoreboard(
    const ProviderResponse& response,
    const std::string* favoriteTeamIds,
    size_t favoriteCount);

void renderSimulatorUpcomingGame(
    const Game& game);

void renderSimulatorIdleDashboard(
    const Game* nextFavoriteGame,
    int64_t nowUnix,
    const std::string* favoriteTeamIds,
    size_t favoriteCount);

void renderSimulatorScoreAlert(
    const ScoreChange& scoreChange);

} 