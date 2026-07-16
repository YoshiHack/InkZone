#include "inkzone/view_mode_controller.h"

namespace inkzone {

ViewModeController::ViewModeController(uint32_t auto_return_ms)
    : auto_return_ms_(auto_return_ms) {}

void ViewModeController::next(size_t view_count, uint32_t now_ms) {
  if (view_count == 0) {
    return;
  }

  selected_view_ = (selected_view_ + 1) % view_count;
  beginManualMode(now_ms);
}

void ViewModeController::previous(size_t view_count, uint32_t now_ms) {
  if (view_count == 0) {
    return;
  }

  selected_view_ = selected_view_ == 0 ? view_count - 1 : selected_view_ - 1;
  beginManualMode(now_ms);
}

void ViewModeController::select(size_t view_index, size_t view_count,
                                uint32_t now_ms) {
  if (view_count == 0) {
    return;
  }

  selected_view_ = view_index % view_count;
  beginManualMode(now_ms);
}

void ViewModeController::enterAutomaticMode() {
  automatic_ = true;
}

void ViewModeController::update(uint32_t now_ms) {
  if (!automatic_ && now_ms - last_interaction_ms_ >= auto_return_ms_) {
    enterAutomaticMode();
  }
}

bool ViewModeController::isAutomatic() const {
  return automatic_;
}

size_t ViewModeController::selectedView() const {
  return selected_view_;
}

void ViewModeController::beginManualMode(uint32_t now_ms) {
  automatic_ = false;
  last_interaction_ms_ = now_ms;
}

}  // namespace inkzone
