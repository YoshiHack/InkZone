#pragma once

namespace inkzone {

enum class WifiConnectResult {
  kSuccess,
  kTimeout,
  kInvalidNetwork,
};

WifiConnectResult connectToWifi(const char* networkName,
                                const char* password,
                                unsigned long timeoutMs);

const char* wifiConnectResultName(WifiConnectResult result);

}  // namespace inkzone