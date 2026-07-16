#pragma once

#include <stddef.h>
#include <stdint.h>

namespace inkzone {

class ViewModeController {
 public:
  explicit ViewModeController(uint32_t auto_return_ms = 5UL * 60UL * 1000UL);

  void next(size_t view_count, uint32_t now_ms);
  void previous(size_t view_count, uint32_t now_ms);
  void select(size_t view_index, size_t view_count, uint32_t now_ms);
  void enterAutomaticMode();
  void update(uint32_t now_ms);

  bool isAutomatic() const;
  size_t selectedView() const;

 private:
  void beginManualMode(uint32_t now_ms);

  uint32_t auto_return_ms_;
  uint32_t last_interaction_ms_ = 0;
  size_t selected_view_ = 0;
  bool automatic_ = true;
};

}  // namespace inkzone
