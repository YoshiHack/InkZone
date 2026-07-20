#include "inkzone/wifi_connection.h"

#include <Arduino.h>
#include <WiFi.h>

namespace inkzone {

WifiConnectResult connectToWifi(
    const char* networkName,
    const char* password,
    unsigned long timeoutMs) {

  if (networkName == nullptr ||
      networkName[0] == '\0') {
    return WifiConnectResult::kInvalidNetwork;
  }

  WiFi.mode(WIFI_STA);

  if (password == nullptr ||
      password[0] == '\0') {
    WiFi.begin(networkName);
  } else {
    WiFi.begin(networkName, password);
  }

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime >= timeoutMs) {
      return WifiConnectResult::kTimeout;
    }

    delay(100);
  }

  return WifiConnectResult::kSuccess;
}

bool startSetupAccessPoint(const char* networkName) {
  if (networkName == nullptr ||
      networkName[0] == '\0') {
    return false;
  }

  WiFi.mode(WIFI_AP);

  if (WiFi.softAP(networkName)) {
    return true;
  }

  return false;
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

}