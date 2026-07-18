#include "inkzone/favorite_team_navigator.h"

namespace inkzone {

void FavoriteTeamNavigator::setTeamCount(uint8_t team_count) {
  team_count_ = team_count;

  if (team_count_ == 0) {
    current_index_ = 0;
  } else if (current_index_ >= team_count_) {
    current_index_ = team_count_ - 1;
  }
}

uint8_t FavoriteTeamNavigator::teamCount() const {
  return team_count_;
}

uint8_t FavoriteTeamNavigator::currentIndex() const {
  return current_index_;
}

bool FavoriteTeamNavigator::hasTeams() const {
  return team_count_ > 0;
}

bool FavoriteTeamNavigator::next() {
  if (team_count_ <= 1) {
    return false;
  }

  current_index_ = (current_index_ + 1) % team_count_;
  return true;
}

bool FavoriteTeamNavigator::previous() {
  if (team_count_ <= 1) {
    return false;
  }

  current_index_ =
      current_index_ == 0 ? team_count_ - 1 : current_index_ - 1;
  return true;
}

bool FavoriteTeamNavigator::select(uint8_t index) {
  if (index >= team_count_ || index == current_index_) {
    return false;
  }

  current_index_ = index;
  return true;
}

}
