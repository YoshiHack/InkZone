#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "inkzone/models.h"

namespace inkzone {

struct SportsDataSnapshot {
  std::string provider_name;
  int64_t updated_at_unix = 0;
  std::vector<Game> games;
  std::vector<Standing> standings;
};

class SportsDataCache {
 public:
  bool storeIfValid(SportsDataSnapshot snapshot);
  bool hasData() const;
  const SportsDataSnapshot& latest() const;

 private:
  bool has_data_ = false;
  SportsDataSnapshot latest_;
};

}  // namespace inkzone
