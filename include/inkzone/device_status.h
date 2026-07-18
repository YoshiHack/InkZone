#pragma once

#include <stdint.h>

namespace inkzone
{

enum class DeviceState
{
    kStarting,
    kNeedsSetup,
    kConnecting,
    kFetchingData,
    kReady,
    kOffline
};

class DeviceStatus
{
public:
    DeviceState state() const;
    uint32_t changedAtMs() const;
    bool isOperational() const;

    void transitionTo(
        DeviceState state,
        uint32_t now_ms);

private:
    DeviceState state_ = DeviceState::kStarting;
    uint32_t changed_at_ms_ = 0;
};

const char* deviceStateName(DeviceState state);

}