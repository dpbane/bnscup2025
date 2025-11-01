#include "enemy_level_designer.hpp"

namespace bnscup2025::enemy {

EnemyParameters EnemyLevelDesigner::MakeParameters(int level) {
  constexpr double sound_hear_radius_min = 20.0;
  constexpr double sound_hear_radius_max = 30.0;
  constexpr double view_radius_min = 20.0;
  constexpr double view_radius_max = 25.0;
  constexpr double view_fov_min = 60.0_deg;
  constexpr double view_fov_max = 120.0_deg;
  const double view_fov_cos_min = Cos(view_fov_min / 2.0);
  const double view_fov_cos_max = Cos(view_fov_max / 2.0);
  constexpr double prowl_speed_min = 2.6;
  constexpr double prowl_speed_max = 4.0;
  constexpr double to_sound_speed_min = 2.8;
  constexpr double to_sound_speed_max = 3.0;
  constexpr double pursuit_speed_min = 3.0;
  constexpr double pursuit_speed_max = 7.0;

  double ratio = (level - 1) / (12.0 - 1.0);  // レベル1から12までを想定

  return EnemyParameters {
    .sound_hear_radius = sound_hear_radius_min + (sound_hear_radius_max - sound_hear_radius_min) * ratio,
    .view_radius = view_radius_min + (view_radius_max - view_radius_min) * ratio,
    .view_fov_cos = view_fov_cos_min + (view_fov_cos_max - view_fov_cos_min) * ratio,
    .prowl_speed = prowl_speed_min + (prowl_speed_max - prowl_speed_min) * ratio,
    .to_sound_speed = to_sound_speed_min + (to_sound_speed_max - to_sound_speed_min) * ratio,
    .pursuit_speed = pursuit_speed_min + (pursuit_speed_max - pursuit_speed_min) * ratio
  };

}

}
