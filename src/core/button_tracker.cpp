#include "inkzone/button_tracker.h"

namespace inkzone {

ButtonTracker::ButtonTracker(uint32_t debounce_ms, uint32_t long_press_ms)
    : debounce_ms_(debounce_ms), long_press_ms_(long_press_ms) {}

ButtonEvent ButtonTracker::update(bool is_pressed, uint32_t now_ms) {
  if (is_pressed != raw_pressed_) {
    raw_pressed_ = is_pressed;
    raw_changed_at_ms_ = now_ms;
  }

  if (raw_pressed_ != stable_pressed_ &&
      now_ms - raw_changed_at_ms_ >= debounce_ms_) {
    stable_pressed_ = raw_pressed_;

    if (stable_pressed_) {
      pressed_at_ms_ = now_ms;
      long_press_reported_ = false;
    } else if (!long_press_reported_) {
      return ButtonEvent::kShortPress;
    }
  }

  if (stable_pressed_ && !long_press_reported_ &&
      now_ms - pressed_at_ms_ >= long_press_ms_) {
    long_press_reported_ = true;
    return ButtonEvent::kLongPress;
  }

  return ButtonEvent::kNone;
}

void ButtonTracker::reset() {
  raw_changed_at_ms_ = 0;
  pressed_at_ms_ = 0;
  raw_pressed_ = false;
  stable_pressed_ = false;
  long_press_reported_ = false;
}

}  // namespace inkzone

