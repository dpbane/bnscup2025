#pragma once

#include "enemy.hpp"

namespace bnscup2025::enemy {

class EnemyLevelDesigner {
  EnemyLevelDesigner() = delete;
  EnemyLevelDesigner(const EnemyLevelDesigner&) = delete;
  EnemyLevelDesigner& operator=(const EnemyLevelDesigner&) = delete;

public:
  static EnemyParameters MakeParameters(int level);
};

}
