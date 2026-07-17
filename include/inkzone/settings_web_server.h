#pragma once

#include <WebServer.h>

#include "inkzone/settings.h"

namespace inkzone {

  enum class WebsiteViewCommand {
  kNone,
  kPrevious,
  kAutomatic,
  kNext,
};
class SettingsWebServer {
 public:
  explicit SettingsWebServer(Settings& settings,
                             unsigned short port = 90);

    WebsiteViewCommand takeViewCommand();

  void begin();
  void handleClient();
  bool isRunning() const;

 private:
  void handleHome();
  void handleSave();
  void handleReset();

  void handleViewCommand();

WebsiteViewCommand pending_view_command_ =
    WebsiteViewCommand::kNone;

  Settings& settings_;
  WebServer server_;
  bool running_ = false;
};

}  // namespace inkzone