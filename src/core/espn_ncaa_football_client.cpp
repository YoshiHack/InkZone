#include "inkzone/espn_ncaa_football_client.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "inkzone/espn_nfl_parser.h"

namespace inkzone {
namespace {

constexpr char kNcaaFootballScoreboardUrl[] =
    "https://site.api.espn.com/apis/site/v2/sports/"
    "football/college-football/scoreboard?limit=5";

}

ProviderResponse fetchEspnNcaaFootballScoreboard() {
  ProviderResponse response;

  if (WiFi.status() != WL_CONNECTED) {
    response.result = ProviderResult::kNetworkUnavailable;
    response.diagnostic = "Wi-Fi is not connected";
    return response;
  }

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(20000);

  HTTPClient request;
  request.setTimeout(20000);

  if (!request.begin(client, kNcaaFootballScoreboardUrl)) {
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic = "Could not begin NCAA football request";
    return response;
  }

  request.useHTTP10(true);

  const int statusCode = request.GET();

  if (statusCode == 429) {
    request.end();
    response.result = ProviderResult::kRateLimited;
    response.diagnostic = "NCAA football provider rate limited the request";
    return response;
  }

  if (statusCode != HTTP_CODE_OK) {
    request.end();
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic =
        "NCAA football request returned HTTP " +
        std::to_string(statusCode);
    return response;
  }

const String json = request.getString();

request.end();

return parseEspnScoreboard(
    json.c_str(),
    League::kNcaaFootball);
}

}