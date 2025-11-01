#include "stdafx.h"
#include "level_designer.hpp"

namespace bnscup2025::terrain {

LevelDesignerParameters LevelDesigner::MakeParameters(int level) {
  Size size = Size { 70, 70 } + (level - 1) * Size { 10, 10 };


  return LevelDesignerParameters {
    .size = size,
    .biggest_area_ratio_min = 0.25,
    .biggest_area_ratio_max = 0.30,
    .enemy_distance = 30.0,
    .sinhalite_count = 20 + (level - 1) * 3
  };
}

}
