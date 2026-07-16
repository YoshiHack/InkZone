#pragma once

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "inkzone/models.h"
#include "inkzone/refresh_scheduler.h"

namespace inkzone {

struct RefreshPlan {
  ActivityLevel activity = ActivityLevel::kNoGamesToday;
  uint32_t interval_ms = 0;
};

RefreshPlan planRefresh(const Game* games, size_t game_count,
                        const std::string* favorite_team_ids,
                        size_t favorite_count, int64_t now_unix,
                        bool network_available, uint8_t retry_attempt = 0);

const char* activityLevelName(ActivityLevel activity);

}  // namespace inkzone
