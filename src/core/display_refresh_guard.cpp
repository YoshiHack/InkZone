#include "inkzone/display_refresh_guard.h"

namespace inkzone {

DisplayRefreshGuard::DisplayRefreshGuard(uint32_t minimum_gap_ms)
    : minimum_gap_ms_(minimum_gap_ms) {}

bool DisplayRefreshGuard::canStart(bool panel_busy, uint32_t now_ms) const {
  if (panel_busy || refresh_in_progress_) {
    return false;
  }

  if (!has_completed_refresh_) {
    return true;
  }

  return now_ms - last_completed_at_ms_ >= minimum_gap_ms_;
}

bool DisplayRefreshGuard::start(bool panel_busy, uint32_t now_ms) {
  if (!canStart(panel_busy, now_ms)) {
    return false;
  }

  refresh_in_progress_ = true;
  return true;
}

void DisplayRefreshGuard::finish(uint32_t now_ms) {
  if (!refresh_in_progress_) {
    return;
  }

  refresh_in_progress_ = false;
  has_completed_refresh_ = true;
  last_completed_at_ms_ = now_ms;
}

bool DisplayRefreshGuard::refreshInProgress() const {
  return refresh_in_progress_;
}

uint32_t DisplayRefreshGuard::lastCompletedAtMs() const {
  return last_completed_at_ms_;
}

}  // namespace inkzone
