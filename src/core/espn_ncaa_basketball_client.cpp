#include "inkzone/espn_ncaa_basketball_client.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "inkzone/espn_nfl_parser.h"

namespace inkzone {
namespace {

constexpr char kNcaaBasketballScoreboardUrl[] =
    "https://site.api.espn.com/apis/site/v2/sports/"
    "basketball/mens-college-basketball/scoreboard?limit=5";

}

ProviderResponse fetchEspnNcaaBasketballScoreboard() {
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

  if (!request.begin(client, kNcaaBasketballScoreboardUrl)) {
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic =
        "Could not begin NCAA basketball request";
    return response;
  }

  const int statusCode = request.GET();

  if (statusCode == 429) {
    request.end();
    response.result = ProviderResult::kRateLimited;
    response.diagnostic =
        "NCAA basketball provider rate limited the request";
    return response;
  }

  if (statusCode != HTTP_CODE_OK) {
    request.end();
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic =
        "NCAA basketball request returned HTTP " +
        std::to_string(statusCode);
    return response;
  }

  const String json = request.getString();
  request.end();

  if (json.isEmpty()) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic =
        "NCAA basketball provider returned an empty response";
    return response;
  }

  return parseEspnScoreboard(
      json.c_str(),
      League::kNcaaBasketball);
}

}  // namespace inkzone