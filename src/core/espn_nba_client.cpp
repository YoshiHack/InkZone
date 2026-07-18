#include "inkzone/espn_nba_client.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "inkzone/espn_nfl_parser.h"

namespace inkzone {
namespace {

constexpr char kNbaScoreboardUrl[] =
    "https://site.api.espn.com/apis/site/v2/sports/"
    "basketball/nba/scoreboard?limit=1";

}  // namespace

ProviderResponse fetchEspnNbaScoreboard() {
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
  request.useHTTP10(true);

  if (!request.begin(client, kNbaScoreboardUrl)) {
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic = "Could not begin NBA request";
    return response;
  }

  const int statusCode = request.GET();

  if (statusCode == 429) {
    request.end();
    response.result = ProviderResult::kRateLimited;
    response.diagnostic = "NBA provider rate limited the request";
    return response;
  }

  if (statusCode != HTTP_CODE_OK) {
    request.end();
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic =
        "NBA request returned HTTP " +
        std::to_string(statusCode);
    return response;
  }

  const String json = request.getString();
  request.end();

  if (json.isEmpty()) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic =
        "NBA provider returned an empty response";
    return response;
  }

  return parseEspnScoreboard(
      json.c_str(),
      League::kNba);
}

} 