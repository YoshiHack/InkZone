#pragma once

#include <WebServer.h>

#include "inkzone/settings.h"

namespace inkzone {

class SettingsWebServer {
 public:
  explicit SettingsWebServer(Settings& settings,
                             unsigned short port = 90);

  void begin();
  void handleClient();
  bool isRunning() const;

 private:
  void handleHome();
  void handleSave();
  void handleReset();

  Settings& settings_;
  WebServer server_;
  bool running_ = false;
};

}  // namespace inkzone