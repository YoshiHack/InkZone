#include <Arduino.h>

namespace {
constexpr unsigned long kSerialBaudRate = 115200;
constexpr unsigned long kHeartbeatIntervalMs = 30000;
unsigned long lastHeartbeatMs = 0;

void printBootInformation() {
  Serial.printf("Chip: %s, revision %d\n", ESP.getChipModel(),
                ESP.getChipRevision());
  Serial.printf("CPU: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash: %u bytes\n", ESP.getFlashChipSize());
}
}  // namespace

void setup() {
  Serial.begin(kSerialBaudRate);
  delay(500);

  Serial.println();
  Serial.println("InkZone starting...");
  printBootInformation();
  Serial.println("Serial connection ready.");
}

void loop() {
  const unsigned long nowMs = millis();

  if (nowMs - lastHeartbeatMs >= kHeartbeatIntervalMs) {
    lastHeartbeatMs = nowMs;
    Serial.printf("InkZone uptime: %lu seconds\n", nowMs / 1000);
  }

  delay(10);
}
