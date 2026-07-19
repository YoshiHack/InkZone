#include "inkzone/settings_web_server.h"
#include "inkzone/settings_storage.h"

#include <Arduino.h>
#include <WiFi.h>


namespace inkzone {
namespace {

const char kSettingsPage[] = R"html(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>InkZone Settings</title>
  <style>
    body {
      margin: 0;
      padding: 24px;
      background: #111827;
      color: #f9fafb;
      font-family: Arial, sans-serif;
    }

    main {
      max-width: 560px;
      margin: 0 auto;
      padding: 24px;
      background: #1f2937;
      border-radius: 16px;
    }

    h1 {
      margin-top: 0;
    }

    label {
      display: block;
      margin-top: 18px;
      margin-bottom: 6px;
      font-weight: bold;
    }

    input,
    select,
    button {
      box-sizing: border-box;
      width: 100%;
      padding: 12px;
      border: 0;
      border-radius: 8px;
      font-size: 16px;
    }

    button {
      margin-top: 24px;
      background: #f97316;
      color: white;
      font-weight: bold;
      cursor: pointer;
    }

    .status {
      padding: 12px;
      background: #064e3b;
      border-radius: 8px;
    }
    
        .view-controls {
      margin-top: 28px;
      padding-top: 20px;
      border-top: 1px solid #374151;
    }

    .view-buttons {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 8px;
    }

    .view-buttons form {
      margin: 0;
    }

    .view-buttons button {
      margin-top: 0;
      background: #2563eb;
    }
  </style>
</head>
<body>
  <main>
    <h1>InkZone Settings</h1>
    <p class="status">%NETWORK_STATUS%</p>
    <form method="post" action="/save">
                <label for="wifi_ssid">Wi-Fi network</label>
      <input id="wifi_ssid"
             name="wifi_ssid"
             list="wifi_networks"
             maxlength="32"
             value="%WIFI_SSID%"
             placeholder="Select or type a network name">

      <datalist id="wifi_networks">
        %WIFI_NETWORK_OPTIONS%
      </datalist>
      <label for="timezone">Timezone</label>
      <select id="timezone" name="timezone">
        <option value="America/New_York" %TZ_EASTERN%>Eastern</option>  
        <option value="America/Chicago" %TZ_CENTRAL%>Central</option>
        <option value="America/Denver" %TZ_MOUNTAIN%>Mountain</option>
        <option value="America/Los_Angeles" %TZ_PACIFIC%>Pacific</option>
      </select>

            <label for="league">Favorite league</label>
      <select id="league" name="league">
        <option value="nfl" %LEAGUE_NFL%>NFL</option>
        <option value="ncaa_football" %LEAGUE_NCAA_FOOTBALL%>
          NCAA football
        </option>
        <option value="nba" %LEAGUE_NBA%>NBA</option>
        <option value="ncaa_basketball" %LEAGUE_NCAA_BASKETBALL%>
          NCAA basketball
        </option>
        <option value="nhl" %LEAGUE_NHL%>NHL</option>
      </select>
      <label for="team">Favorite team abbreviation</label>
      <input id="team"
             name="team"
             maxlength="5"
             value="%TEAM%"
             placeholder="Example: BUF">

      <label for="interval">Update interval in seconds</label>
      <input id="interval"
             name="interval"
             type="number"
             min="30"
             max="300"
             value="%INTERVAL%">

      <button type="submit">Save settings</button>
    </form>
    <section class="view-controls">
  <h2>Display controls</h2>

  <div class="view-buttons">
    <form method="post" action="/view">
      <button name="action" value="previous" type="submit">
        Previous
      </button>
    </form>

    <form method="post" action="/view">
      <button name="action" value="automatic" type="submit">
        Auto
      </button>
    </form>

    <form method="post" action="/view">
      <button name="action" value="next" type="submit">
        Next
      </button>
    </form>
  </div>
</section>
    <form action="/reset" method="POST"
      onsubmit="return confirm('Reset all saved InkZone settings?');">
  <button type="submit" class="reset-button">
    Reset saved settings
  </button>
</form>
  </main>
</body>
</html>
)html";

const char kSavedPage[] = R"html(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Settings Saved</title>
</head>
<body>
  <h1>Settings saved</h1>
  <p>InkZone received the new settings.</p>
  <p><a href="/">Return to settings</a></p>
</body>
</html>
)html";

String buildWiFiNetworkOptions(const std::string& selectedSsid) {
  String options;

  const int networkCount = WiFi.scanNetworks();

  if (networkCount <= 0) {
    return "<option value=\"\">No networks found</option>";
  }

  for (int index = 0; index < networkCount; ++index) {
    const String ssid = WiFi.SSID(index);

    if (ssid.isEmpty()) {
      continue;
    }

    options += "<option value=\"";
    options += ssid;
    options += "\"";

    if (ssid == selectedSsid.c_str()) {
      options += " selected";
    }

    options += ">";
    options += ssid;
    options += "</option>";
  }

  WiFi.scanDelete();
  return options;
}

}  // namespace

SettingsWebServer::SettingsWebServer(Settings& settings,
                                     unsigned short port)
    : settings_(settings),
      server_(port) {}

void SettingsWebServer::begin() {
  server_.on("/", HTTP_GET, [this]() {
    handleHome();
  });

  server_.on("/save", HTTP_POST, [this]() {
    handleSave();
  });

server_.on("/reset", HTTP_POST, [this]() {
  handleReset();
});

server_.on("/view", HTTP_POST, [this]() {
  handleViewCommand();
});

  server_.onNotFound([this]() {
    server_.send(404, "text/plain", "Page not found");
  });

  server_.begin();
  running_ = true;

  Serial.println("Settings website started on port 90");
}

void SettingsWebServer::handleClient() {
  if (running_) {
    server_.handleClient();
  }
}

bool SettingsWebServer::isRunning() const {
  return running_;
}

void SettingsWebServer::handleHome() {
  String page = kSettingsPage;

  page.replace(
      "%TZ_EASTERN%",
      settings_.timezone == "America/New_York" ? "selected" : "");

  page.replace(
      "%TZ_CENTRAL%",
      settings_.timezone == "America/Chicago" ? "selected" : "");

  page.replace(
      "%TZ_MOUNTAIN%",
      settings_.timezone == "America/Denver" ? "selected" : "");

  page.replace(
      "%TZ_PACIFIC%",
      settings_.timezone == "America/Los_Angeles" ? "selected" : "");

      const League selectedLeague =
    settings_.favorite_leagues.empty()
        ? League::kNfl
        : settings_.favorite_leagues.front();

page.replace(
    "%LEAGUE_NFL%",
    selectedLeague == League::kNfl ? "selected" : "");

page.replace(
    "%LEAGUE_NCAA_FOOTBALL%",
    selectedLeague == League::kNcaaFootball ? "selected" : "");

page.replace(
    "%LEAGUE_NBA%",
    selectedLeague == League::kNba ? "selected" : "");

page.replace(
    "%LEAGUE_NCAA_BASKETBALL%",
    selectedLeague == League::kNcaaBasketball ? "selected" : "");

page.replace(
    "%LEAGUE_NHL%",
    selectedLeague == League::kNhl ? "selected" : "");
  const char* team =
      settings_.favorite_team_ids.empty()
          ? ""
          : settings_.favorite_team_ids.front().c_str();

  page.replace("%WIFI_SSID%", settings_.wifi_ssid.c_str()); 
  page.replace(
    "%WIFI_NETWORK_OPTIONS%",
    buildWiFiNetworkOptions(settings_.wifi_ssid));       
  page.replace("%TEAM%", team);
  page.replace(
      "%INTERVAL%",
      String(settings_.live_refresh_seconds));

  const bool connectedToWifi =
    WiFi.status() == WL_CONNECTED;

page.replace(
    "%NETWORK_STATUS%",
    connectedToWifi
        ? "InkZone is connected to Wi-Fi."
        : "Setup mode: choose your home Wi-Fi network.");

  server_.send(200, "text/html", page);
}

void SettingsWebServer::handleSave() {
  String timezone = server_.arg("timezone");
  String team = server_.arg("team");
  String league = server_.arg("league");
  String wifiSsid = server_.arg("wifi_ssid");
String wifiPassword = server_.arg("wifi_password");
  const unsigned long interval =
      server_.arg("interval").toInt();

  team.trim();
  team.toUpperCase();

  wifiSsid.trim();

if (!wifiSsid.isEmpty()) {
  settings_.wifi_ssid = wifiSsid.c_str();
}

if (!wifiPassword.isEmpty()) {
  settings_.wifi_password = wifiPassword.c_str();
}

  settings_.timezone = timezone.c_str();

  League selectedLeague = League::kNfl;

if (league == "ncaa_football") {
  selectedLeague = League::kNcaaFootball;
} else if (league == "nba") {
  selectedLeague = League::kNba;
} else if (league == "ncaa_basketball") {
  selectedLeague = League::kNcaaBasketball;
} else if (league == "nhl") {
  selectedLeague = League::kNhl;
}
  settings_.favorite_leagues.clear();
  settings_.favorite_leagues.push_back(selectedLeague);

  settings_.favorite_team_ids.clear();

  if (!team.isEmpty()) {
    settings_.favorite_team_ids.push_back(team.c_str());
  }

  settings_.live_refresh_seconds = interval;

  const SettingsError error = validateSettings(settings_);

  if (error != SettingsError::kNone) {
    server_.send(400,
                 "text/plain",
                 settingsErrorName(error));
    return;
  }

  if (!saveSettings(settings_)) {
    server_.send(500,
                 "text/plain",
                 "Unable to save settings");
    return;
  }

  Serial.printf(
      "Settings saved: timezone=%s, team=%s, interval=%lu\n",
      settings_.timezone.c_str(),
      team.c_str(),
      interval);

  server_.send(200, "text/html", kSavedPage);

  delay(1000);
ESP.restart();
}

WebsiteViewCommand
SettingsWebServer::takeViewCommand() {
  const WebsiteViewCommand command =
      pending_view_command_;

  pending_view_command_ =
      WebsiteViewCommand::kNone;

  return command;
}

void SettingsWebServer::handleViewCommand() {
  const String action = server_.arg("action");

  if (action == "previous") {
    pending_view_command_ =
        WebsiteViewCommand::kPrevious;
  } else if (action == "next") {
    pending_view_command_ =
        WebsiteViewCommand::kNext;
  } else if (action == "automatic") {
    pending_view_command_ =
        WebsiteViewCommand::kAutomatic;
  }

  server_.sendHeader("Location", "/");
  server_.send(303, "text/plain", "");
}

void SettingsWebServer::handleReset() {
  clearSavedSettings();

  server_.send(
      200,
      "text/html",
      "<!DOCTYPE html>"
      "<html><body>"
      "<h1>InkZone settings reset</h1>"
      "<p>The device is restarting.</p>"
      "</body></html>");

  delay(1000);
  ESP.restart();
}

}  // namespace inkzone