#pragma once

#include <stdint.h>

namespace inkzone {

class FavoriteTeamNavigator {
 public:
  void setTeamCount(uint8_t team_count);
  uint8_t teamCount() const;
  uint8_t currentIndex() const;
  bool hasTeams() const;

  bool next();
  bool previous();
  bool select(uint8_t index);

 private:
  uint8_t team_count_ = 0;
  uint8_t current_index_ = 0;
};

}  // namespace inkzone
