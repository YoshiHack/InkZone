#pragma once

#include <stdint.h>

namespace inkzone {

enum class ButtonEvent {
  kNone,
  kShortPress,
  kLongPress,
};

class ButtonTracker {
 public:
  explicit ButtonTracker(uint32_t debounce_ms = 40,
                         uint32_t long_press_ms = 800);

  ButtonEvent update(bool is_pressed, uint32_t now_ms);
  void reset();

 private:
  uint32_t debounce_ms_;
  uint32_t long_press_ms_;
  uint32_t raw_changed_at_ms_ = 0;
  uint32_t pressed_at_ms_ = 0;
  bool raw_pressed_ = false;
  bool stable_pressed_ = false;
  bool long_press_reported_ = false;
};

}  // namespace inkzone

