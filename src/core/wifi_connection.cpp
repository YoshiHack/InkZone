#include "inkzone/wifi_connection.h"

#include <Arduino.h>
#include <WiFi.h>

namespace inkzone {

WifiConnectResult connectToWifi(const char* networkName,
                                const char* password,
                                unsigned long timeoutMs) {
  if (networkName == nullptr || networkName[0] == '\0') {
    return WifiConnectResult::kInvalidNetwork;
  }

  WiFi.mode(WIFI_STA);

  if (password == nullptr || password[0] == '\0') {
    WiFi.begin(networkName);
  } else {
    WiFi.begin(networkName, password);
  }

  const unsigned long startedAtMs = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startedAtMs >= timeoutMs) {
      return WifiConnectResult::kTimeout;
    }

    delay(100);
  }

  return WifiConnectResult::kSuccess;
}

bool startSetupAccessPoint(const char* networkName) {
  if (networkName == nullptr || networkName[0] == '\0') {
    return false;
  }

  WiFi.mode(WIFI_AP);
  return WiFi.softAP(networkName);
}

const char* wifiConnectResultName(WifiConnectResult result) {
  switch (result) {
    case WifiConnectResult::kSuccess:
      return "success";
    case WifiConnectResult::kTimeout:
      return "timeout";
    case WifiConnectResult::kInvalidNetwork:
      return "invalid network";
  }

  return "unknown";
}

}  // namespace inkzone