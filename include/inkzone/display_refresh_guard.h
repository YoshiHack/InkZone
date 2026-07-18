#pragma once

#include <stdint.h>

namespace inkzone
{

class DisplayRefreshGuard
{
public:
    explicit DisplayRefreshGuard(uint32_t minimum_gap_ms = 5000);

    bool canStart(bool panel_busy, uint32_t now_ms) const;
    bool start(bool panel_busy, uint32_t now_ms);
    void finish(uint32_t now_ms);

    bool refreshInProgress() const;
    uint32_t lastCompletedAtMs() const;

private:
    uint32_t minimum_gap_ms_;
    uint32_t last_completed_at_ms_ = 0;
    bool has_completed_refresh_ = false;
    bool refresh_in_progress_ = false;
};

}