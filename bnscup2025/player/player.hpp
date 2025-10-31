#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"
#include "power_grade.hpp"
#include "grade_value_converter.hpp"
#include "goshin/selector.hpp"

#include "energy_ui.hpp"

namespace bnscup2025::player {

class Player {
public:
  Player(const camera::Camera& camera, terrain::Terrain& terrain, Effect& effect, Vec2 pos, bool is_game, const PowerGrade& power_grade);

  void Update();
  void Render() const;
  void RenderUI(double energy_radius) const;

  const Vec2& GetPosition() const { return position_; }
  const Vec2 GetShiftedPosition() const { return position_ + shift_amount_; }
  const Vec2& GetDirectionFace() const { return direction_face_; }

  const Optional<Vec2>& GetSoundPosition() const { return sound_position_; }
  const PowerGrade& GetPowerGrade() const { return power_grade_; }

  double GetKokoroAlpha() const { return kokoro_alpha_; }
  bool IsTsutsuActive() const { return is_tsutsu_active_; }
  bool IsBurnout() const { return burnout_timer_ > 0.0; }

private:
  void ProcessDirectionFace();
  void ProcessSkill();
  void ProcessEnergy();
  void ProcessMove();
  void ProcessDigging();

private:
  void ProcessNobiru();
  void ProcessKokoro();
  void ProcessSusumu();
  void ProcessTsutsu();

private:
  const camera::Camera& camera_;
  terrain::Terrain& terrain_;
  Effect& effect_;
  const bool is_game_;
  const PowerGrade& power_grade_;
  const GradeValueConverter gvc_ { power_grade_ };
  Selector selector_ { power_grade_ };

  Vec2 position_;
  Vec2 direction_face_ { 0.0, -1.0 };

  double energy_ { 10000.0 };  // エネルギー残量
  double move_speed_rate_ { 1.0 };  // 移動速度倍率
  double energy_regen_timer_ { 0.0 };  // エネルギーが回復可能になるまでのタイマー
  double burnout_timer_ { 0.0 };  // バーンアウトが回復するまでのタイマー
  const double burnout_duration_ { 40.0 };  // バーンアウトの持続時間
  bool can_change_skill_ { true };

  Vec2 shift_target_ { 0.0, 0.0 };
  Vec2 shift_amount_ { 0.0, 0.0 };

  bool is_kokoro_active_ { false };
  bool is_tsutsu_active_ { false };
  bool prev_kokoro_active_ { false };
  bool prev_tsutsu_active_ { false };
  double kokoro_minimum_timer_ { 0.0 };
  double kokoro_alpha_ { 0.0 };
  double tsutsu_alpha_ { 0.0 };

  double dig_timer_ { 0.0 };
  Optional<Vec2> digging_position_;

  Optional<Vec2> sound_position_;
  EnergyUI energy_ui_ { power_grade_, energy_ };


};

}
