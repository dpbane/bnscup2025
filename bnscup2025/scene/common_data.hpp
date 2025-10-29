#pragma once

#include "player/power_grade.hpp"

namespace bnscup2025::scene {

enum class Room : uint8 {
  Game,
  Shop
};

struct CommonData {
  int next_level;
  Room next_room;
  player::PowerGrade power_grade;
};

}
