#include "stdafx.h"
#include "player.hpp"

#include "input/input.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

#include "effect/dig.hpp"

namespace bnscup2025::player {

Player::Player(const camera::Camera& camera, terrain::Terrain& terrain, Effect& effect, Vec2 pos) :
  camera_(camera),
  terrain_(terrain),
  effect_(effect),
  position_(pos) {
}

void Player::Update() {
  const auto input_data = input::Input::GetInstance().GetData();
  direction_face_ = input_data.direction_face;

  ProcessMove();
  ProcessShift();
  ProcessDigging();
}

void Player::Render() const {
  const auto& lightbloom = render::LightBloom::GetInstance();
  const ColorF color_edge { 0.2, 0.9, 0.3 };
  const ColorF color_body { 0.01, 0.00, 0.02 };

  // 本体
  {
    const auto target = lightbloom.CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto t = camera_.CreateRenderTransformer();

    // 体（円）
    Circle { position_, 0.40 }
      .draw(color_body)
      .drawFrame(0.10, color_edge);

    // 目
    const auto& v1 = direction_face_;
    const auto v2 = direction_face_.rotated90();
    const auto p1 = position_ + v1 * 0.15;
    const auto pl1 = p1 + v2 * 0.1;
    const auto pl2 = pl1 + v1 * 0.1;
    const auto pr1 = p1 - v2 * 0.1;
    const auto pr2 = pr1 + v1 * 0.1;
    Line { pl1, pl2 }.draw(LineStyle::RoundCap, 0.10, color_edge);
    Line { pr1, pr2 }.draw(LineStyle::RoundCap, 0.10, color_edge);
  }
  lightbloom.Apply(1.0, 1.0, 1.0);

  // 掘削位置
  if (digging_position_) {
    {
      const auto t = camera_.CreateRenderTransformer();
      Line { position_ + direction_face_ * 0.5, *digging_position_ }.draw(LineStyle::RoundDot, 0.10, ColorF { 0.6, 0.6, 0.0 });
    }
    {
      const auto target = lightbloom.CreateRenderTarget();
      const auto blend = render::BlendMode::AlphaMax();
      const auto t = camera_.CreateRenderTransformer();
      Circle { *digging_position_, 0.35 }.drawFrame(0.20, ColorF { 1.0, 1.0, 0.0 });
    }
    lightbloom.Apply(5.0, 5.0, 5.0);

  }
  else {
    const auto t = camera_.CreateRenderTransformer();
    const Vec2 default_dig_pos = position_ + direction_face_ * 4.0;
    Line { position_ + direction_face_ * 0.5, default_dig_pos }.draw(LineStyle::RoundDot, 0.10, ColorF { 0.25, 0.25, 0.25 });
  }




}

void Player::ProcessMove() {
  const auto input_data = input::Input::GetInstance().GetData();

  const double speed = 5.0;
  if (not input_data.shift) {
    position_ += input_data.direction_move.normalized() * speed * Scene::DeltaTime();
  }
  position_ = terrain_.PushbackService(Circle { position_, 0.45 });
}

void Player::ProcessShift() {
  const auto input_data = input::Input::GetInstance().GetData();

  const double shift_speed = 20.0;
  if (input_data.shift) {
    shift_amount_ += input_data.direction_move.normalized() * shift_speed * Scene::DeltaTime();
  }
  const double alpha = Pow(0.3, Scene::DeltaTime() * 10);
  shift_amount_ = shift_amount_ * alpha;
  shift_amount_ = terrain_.PushbackService(Circle { GetShiftedPosition(), 0.45 }) - position_;
}

void Player::ProcessDigging() {
  const auto input_data = input::Input::GetInstance().GetData();

  dig_timer_ = std::max(0.0, dig_timer_ - Scene::DeltaTime());
  digging_position_ = terrain_.CalcLineCollisionPoint(position_, direction_face_, 4.0);

  if (input_data.dig && digging_position_ && dig_timer_ <= 0.0) {
    terrain_.DigAt(*digging_position_, 2.0, 0.1, 0.01);
    dig_timer_ = 0.2;
    effect_.add<effect::Dig>(camera_, *digging_position_, direction_face_, ColorF { 1.0, 1.0, 1.0 });
  }
}

}
