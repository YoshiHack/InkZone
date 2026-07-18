#include "inkzone/espn_nfl_client.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "inkzone/espn_nfl_parser.h"

namespace inkzone {
namespace {

constexpr char kNflScoreboardUrl[] =
    "https://site.api.espn.com/apis/site/v2/sports/football/nfl/scoreboard?limit=5";

}  // namespace

ProviderResponse fetchEspnNflScoreboard() {
  ProviderResponse response;

  if (WiFi.status() != WL_CONNECTED) {
    response.result = ProviderResult::kNetworkUnavailable;
    response.diagnostic = "Wi-Fi is not connected";
    return response;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient request;

  if (!request.begin(client, kNflScoreboardUrl)) {
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic = "Could not begin NFL request";
    return response;
  }

  const int statusCode = request.GET();

  if (statusCode == 429) {
    request.end();
    response.result = ProviderResult::kRateLimited;
    response.diagnostic = "NFL provider rate limited the request";
    return response;
  }

  if (statusCode != HTTP_CODE_OK) {
    request.end();
    response.result = ProviderResult::kRequestFailed;
    response.diagnostic =
        "NFL request returned HTTP " + std::to_string(statusCode);
    return response;
  }

  const String json = request.getString();
  request.end();

  if (json.isEmpty()) {
    response.result = ProviderResult::kInvalidResponse;
    response.diagnostic = "NFL provider returned an empty response";
    return response;
  }

  return parseEspnNflScoreboard(json.c_str());
}

}