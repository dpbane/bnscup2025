#pragma once

namespace bnscup2025::terrain {

struct LevelDesignerParameters {
  Size size;  // ノードグリッドのサイズ
  double biggest_area_ratio_min;  // 全体サイズに対して最も大きい領域が占める割合の最小値
  double biggest_area_ratio_max;  // 全体サイズに対して最も大きい領域が占める割合の最大値
  double enemy_distance;  // プレイヤーと敵の最小距離
  int sinhalite_count;  // シンハライトの数
};

class LevelDesigner {
  LevelDesigner() = delete;
  LevelDesigner(const LevelDesigner&) = delete;
  LevelDesigner& operator=(const LevelDesigner&) = delete;

public:
  static LevelDesignerParameters MakeParameters(int level);

};

}
