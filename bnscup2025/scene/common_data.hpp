#pragma once

#include "player/power_grade.hpp"

namespace bnscup2025::scene {

enum class Room : uint8 {
  Game,
  Shop
};

struct CommonData {
  int next_level { 0 };
  Room next_room { Room::Game };
  player::PowerGrade power_grade {};

  int sinhalite_amount { 0 };
  int death_count { 0 };
  int clear_count { 0 };

};

}
