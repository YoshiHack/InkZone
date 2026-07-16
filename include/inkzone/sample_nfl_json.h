#pragma once

namespace inkzone {

inline constexpr char kSampleNflScoreboardJson[] = R"json(
{
  "events": [
    {
      "id": "401772510",
      "date": "2026-09-10T00:20:00Z",
      "status": {
        "period": 3,
        "displayClock": "08:42",
        "type": {
          "state": "in",
          "completed": false
        }
      },
      "competitions": [
        {
          "competitors": [
            {
              "homeAway": "home",
              "score": "14",
              "team": {
                "id": "12",
                "displayName": "Kansas City Chiefs",
                "abbreviation": "KC"
              }
            },
            {
              "homeAway": "away",
              "score": "17",
              "team": {
                "id": "2",
                "displayName": "Buffalo Bills",
                "abbreviation": "BUF"
              }
            }
          ]
        }
      ]
    },
        {
      "id": "401772511",
      "date": "2026-09-13T17:00:00Z",
      "status": {
        "period": 0,
        "displayClock": "0:00",
        "type": {
          "state": "pre",
          "completed": false
        }
      },
      "competitions": [
        {
          "competitors": [
            {
              "homeAway": "home",
              "score": "0",
              "team": {
                "id": "3",
                "displayName": "Chicago Bears",
                "abbreviation": "CHI"
              }
            },
            {
              "homeAway": "away",
              "score": "0",
              "team": {
                "id": "9",
                "displayName": "Green Bay Packers",
                "abbreviation": "GB"
              }
            }
          ]
        }
      ]
    },
        {
      "id": "401772512",
      "date": "2026-09-14T20:25:00Z",
      "status": {
        "period": 4,
        "displayClock": "0:00",
        "type": {
          "state": "post",
          "completed": true
        }
      },
      "competitions": [
        {
          "competitors": [
            {
              "homeAway": "home",
              "score": "24",
              "team": {
                "id": "14",
                "displayName": "Los Angeles Rams",
                "abbreviation": "LAR"
              }
            },
            {
              "homeAway": "away",
              "score": "27",
              "team": {
                "id": "25",
                "displayName": "San Francisco 49ers",
                "abbreviation": "SF"
              }
            }
          ]
        }
      ]
    }
  ]
}
)json";

}  // namespace inkzone