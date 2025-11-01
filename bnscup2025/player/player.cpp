#include "stdafx.h"
#include "player.hpp"

#include "input/input.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"
#include "render/chara_renderer.hpp"

#include "effect/dig.hpp"
#include "effect/dig_sinhalite.hpp"

namespace bnscup2025::player {

Player::Player(const camera::Camera& camera, terrain::Terrain& terrain, Effect& effect, Vec2 pos, bool is_game, const PowerGrade& power_grade) :
  camera_(camera),
  terrain_(terrain),
  effect_(effect),
  position_(pos),
  is_game_(is_game),
  power_grade_(power_grade) {

  const auto& input_data = input::Input::GetInstance().GetData();
  direction_face_ = input_data.direction_face.isZero() ? Vec2 { 0.0, -1.0 } : input_data.direction_face.normalized();
}

void Player::Update() {
  sound_position_.reset();

  ProcessDirectionFace();
  ProcessMove();
  ProcessSkill();
  ProcessEnergy();

  if (is_game_) {
    //ProcessShift();
    ProcessDigging();
  }

  energy_ui_.Update();
}

void Player::Render() const {
  const double alpha = 1.0 - tsutsu_alpha_ * 0.8;
  const ColorF tsutsu_color = ColorF { 0.5, 0.8, 1.0, alpha };
  const ColorF body_color_base = IsBurnout() ? ColorF { 0.01, 0.01, 0.01 } : ColorF { 0.01, 0.00, 0.02, alpha };
  const ColorF edge_color_base = IsBurnout() ? ColorF { 0.2, 0.2, 0.2 } : ColorF { 0.2, 0.9, 0.3, alpha };
  const ColorF body_color = body_color_base.lerp(tsutsu_color, tsutsu_alpha_ * 0.9);
  const ColorF edge_color = edge_color_base.lerp(tsutsu_color, tsutsu_alpha_ * 0.9);
  render::CharaRenderer::Render(camera_, position_, direction_face_, body_color, edge_color, 1.0, 1.0, 1.0);

  const auto& lightbloom = render::LightBloom::GetInstance();

  if (is_game_) {
    // 掘削位置
    if (digging_position_) {
      {
        const auto t = camera_.CreateRenderTransformer();
        Line { position_ + direction_face_ * kCharacterRadius * 1.2, *digging_position_ }.draw(LineStyle::RoundDot, 0.25, ColorF { 0.6, 0.6, 0.0 });
      }
      {
        const auto target = lightbloom.CreateRenderTarget();
        const auto blend = render::BlendMode::AlphaMax();
        const auto t = camera_.CreateRenderTransformer();
        Circle { *digging_position_, 0.50 }.drawFrame(0.40, ColorF { 1.0, 1.0, 0.0 });
      }
      lightbloom.Apply(5.0, 5.0, 5.0);

    }
    else {
      const auto t = camera_.CreateRenderTransformer();
      const double dig_distance = gvc_.GetDigDistance();
      const Vec2 default_dig_pos = position_ + direction_face_ * dig_distance;
      Line { position_ + direction_face_ * kCharacterRadius * 1.2, default_dig_pos }.draw(LineStyle::RoundDot, 0.25, ColorF { 0.25, 0.25, 0.25 });
    }
  }

}

void Player::RenderUI(double energy_radius) const {
  selector_.Render();

  if (is_game_) {
    Optional<double> burnout_rate;
    if (IsBurnout()) burnout_rate = burnout_timer_ / burnout_duration_;
    energy_ui_.Render(burnout_rate, energy_radius);
  }
}

void Player::ProcessDirectionFace() {
  const auto& input_data = input::Input::GetInstance().GetData();
  if (input_data.direction_face.isZero()) return;

  const double angle_input = input_data.direction_face.getAngle();
  const double angle_current = direction_face_.getAngle();
  double angle_diff = angle_input - angle_current;
  if (angle_diff > Math::Pi) angle_diff -= Math::TwoPi;
  if (angle_diff < -Math::Pi) angle_diff += Math::TwoPi;

  constexpr double rotate_speed = 720_deg;  // per second
  if (Abs(angle_diff) <= rotate_speed * Scene::DeltaTime()) {
    direction_face_ = input_data.direction_face.normalized();
  }
  else {
    const double angle_new = angle_current + Sign(angle_diff) * rotate_speed * Scene::DeltaTime();
    direction_face_ = Vec2 { 0.0, -1.0 }.rotated(angle_new);
  }
}

void Player::ProcessSkill() {
  move_speed_rate_ = 1.0;
  can_change_skill_ = true;

  ProcessNobiru();
  ProcessKokoro();
  ProcessSusumu();
  ProcessTsutsu();

  selector_.Update(can_change_skill_);
}

void Player::ProcessEnergy() {
  if (burnout_timer_ == 0.0) {
    // バーンアウト中でなければ

    // エネルギー回復タイマーを更新
    if (energy_regen_timer_ > 0.0) {
      energy_regen_timer_ = std::max(0.0, energy_regen_timer_ - Scene::DeltaTime());
    }
    else {
      energy_ += gvc_.GetEnergyRegen() * Scene::DeltaTime();
    }

  }
  else {
    // バーンアウト中ならタイマーを更新
    burnout_timer_ -= Scene::DeltaTime();

    // バーンアウト復帰
    if (burnout_timer_ < 0.0) {
      burnout_timer_ = 0.0;
      energy_ = gvc_.GetMaxEnergy();
    }
  }

  // バーンアウト判定
  if (energy_ < 0.0) {
    energy_ = 0.0;
    burnout_timer_ = burnout_duration_;
  }

  energy_ = std::min(energy_, gvc_.GetMaxEnergy());
}

void Player::ProcessMove() {
  const auto& input_data = input::Input::GetInstance().GetData();

  const double speed = 7.0 * gvc_.GetMoveSpeedRate() * move_speed_rate_;
  position_ += input_data.direction_move.normalized() * speed * Scene::DeltaTime();
  position_ = terrain_.Pushback(Circle { position_, kCharacterRadius });
}

void Player::ProcessDigging() {
  const auto& input_data = input::Input::GetInstance().GetData();


  dig_timer_ = std::max(0.0, dig_timer_ - Scene::DeltaTime());
  const double dig_distance = gvc_.GetDigDistance();
  digging_position_ = terrain_.CalcLineCollisionPoint(position_, direction_face_, dig_distance);

  if (input_data.dig && digging_position_ && dig_timer_ <= 0.0) {
    const double dig_radius = gvc_.GetDigRange();
    const double dig_might_center = 0.15;
    const double dig_might_end = 0.01;
    terrain_.DigAt(*digging_position_, dig_radius, dig_might_center, dig_might_end);
    dig_timer_ = gvc_.GetDigCooldown();
    effect_.add<effect::Dig>(camera_, *digging_position_, direction_face_);
    sound_position_ = (*digging_position_ * 0.2 + position_ * 0.8);

    if (terrain_.GetEarnedSinhalite() > 0) {
      effect_.add<effect::DigSinhalite>(camera_, *digging_position_, direction_face_);
    }
  }
}

void Player::ProcessNobiru() {
  const auto& input_data = input::Input::GetInstance().GetData();

  const double shift_distance = gvc_.GetNobiruShiftDistance();
  const auto item = selector_.GetSelectedItem();
  if (is_game_ && input_data.action && burnout_timer_ == 0.0 && item && *item == PowerGradeItem::Nobiru) {
    // 使用時効果：シフト目標値をセットする
    shift_target_ = input_data.direction_move.normalized() * shift_distance;
    if (not input_data.direction_move.isZero()) {
      if (const auto endpoint_opt = terrain_.CalcLineCollisionPoint(position_, input_data.direction_move.normalized(), shift_distance)) {
        const double distance = endpoint_opt->distanceFrom(position_) * 0.99;
        shift_target_ = input_data.direction_move.normalized() * distance;

        if (shift_amount_.length() > distance) {
          shift_amount_ = shift_amount_.normalized() * distance;
        }
      }
    }

    // 不随効果
    can_change_skill_ = false;
    move_speed_rate_ = 0.0;
    energy_regen_timer_ = 2.0;
  }
  else {
    shift_target_ = Vec2 { 0.0, 0.0 };
  }

  // 常時効果：シフト目標値にシフト量を近づける
  const double shift_alpha = Pow(0.3, Scene::DeltaTime() * 10);
  shift_amount_ = shift_amount_ * shift_alpha + shift_target_ * (1.0 - shift_alpha);

}

void Player::ProcessKokoro() {
  const auto& input_data = input::Input::GetInstance().GetData();

  kokoro_minimum_timer_ = Max(kokoro_minimum_timer_ - Scene::DeltaTime(), 0.0);

  const auto item = selector_.GetSelectedItem();
  if (is_game_ && input_data.action && burnout_timer_ == 0.0 && item && *item == PowerGradeItem::Kokoro) {
    // 使用時効果：心発動
    is_kokoro_active_ = true;

    // 不随効果
    energy_regen_timer_ = 2.0;
    if (not prev_kokoro_active_) {
      // 発動時コスト
      energy_ -= gvc_.GetKokoroBeginCost();
      kokoro_minimum_timer_ = 0.5;  // 最低維持時間
    }
    else if (kokoro_minimum_timer_ == 0.0) {
      // 継続コスト
      energy_ -= gvc_.GetKokoroMaintainCost() * Scene::DeltaTime();
    }
  }
  else {
    is_kokoro_active_ = false;
  }

  // 描画エフェクト用アルファ値更新
  if (is_kokoro_active_ || kokoro_minimum_timer_) {
    kokoro_alpha_ += Scene::DeltaTime() * 5.0;
  }
  else {
    kokoro_alpha_ -= Scene::DeltaTime() * 5.0;
  }
  kokoro_alpha_ = Clamp(kokoro_alpha_, 0.0, 1.0);

  can_change_skill_ = kokoro_alpha_ == 0.0;
  prev_kokoro_active_ = is_kokoro_active_;
}

void Player::ProcessSusumu() {
  const auto& input_data = input::Input::GetInstance().GetData();

  const auto item = selector_.GetSelectedItem();
  if (is_game_ && input_data.action && burnout_timer_ == 0.0 && item && *item == PowerGradeItem::Susumu) {
    // 使用時効果：進発動
    move_speed_rate_ = gvc_.GetSususmuSpeedRate();

    // 不随効果
    can_change_skill_ = false;
    energy_regen_timer_ = 2.0;
    energy_ -= 1500.0 * Scene::DeltaTime();
  }
  else {
  }
}

void Player::ProcessTsutsu() {
  const auto& input_data = input::Input::GetInstance().GetData();

  const auto item = selector_.GetSelectedItem();
  if (is_game_ && input_data.action && burnout_timer_ == 0.0 && item && *item == PowerGradeItem::Tsutsu) {
    // 使用時効果：慎発動
    is_tsutsu_active_ = true;

    // 不随効果
    can_change_skill_ = false;
    move_speed_rate_ = 0.20;
    energy_regen_timer_ = 2.0;
    if (not prev_tsutsu_active_) {
      // 発動時コスト
      energy_ -= gvc_.GetTsutsuStartCost();
    }
    else {
      // 継続コスト
      energy_ -= gvc_.GetTsutsuMaintainCost() * Scene::DeltaTime();
    }
  }
  else {
    is_tsutsu_active_ = false;
  }

  if (is_tsutsu_active_) {
    tsutsu_alpha_ += Scene::DeltaTime() * 3.0;
  }
  else {
    tsutsu_alpha_ -= Scene::DeltaTime() * 3.0;
  }
  tsutsu_alpha_ = Clamp(tsutsu_alpha_, 0.0, 1.0);

  prev_tsutsu_active_ = is_tsutsu_active_;
}

}
