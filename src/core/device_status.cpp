#include "inkzone/device_status.h"

namespace inkzone {

DeviceState DeviceStatus::state() const {
  return state_;
}

uint32_t DeviceStatus::changedAtMs() const {
  return changed_at_ms_;
}

bool DeviceStatus::isOperational() const {
  return state_ == DeviceState::kReady || state_ == DeviceState::kOffline;
}

void DeviceStatus::transitionTo(DeviceState state, uint32_t now_ms) {
  if (state_ == state) {
    return;
  }

  state_ = state;
  changed_at_ms_ = now_ms;
}

const char* deviceStateName(DeviceState state) {
  switch (state) {
    case DeviceState::kStarting:
      return "starting";
    case DeviceState::kNeedsSetup:
      return "needs setup";
    case DeviceState::kConnecting:
      return "connecting";
    case DeviceState::kFetchingData:
      return "fetching data";
    case DeviceState::kReady:
      return "ready";
    case DeviceState::kOffline:
      return "offline";
  }

  return "unknown";
}

}  // namespace inkzone
