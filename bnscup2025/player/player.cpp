#include "stdafx.h"
#include "player.hpp"

#include "input/input.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"
#include "render/chara_renderer.hpp"

#include "effect/dig.hpp"
#include "effect/dig_sinhalite.hpp"

namespace bnscup2025::player {

const double kDigRange = 8.0;

Player::Player(const camera::Camera& camera, terrain::Terrain& terrain, Effect& effect, Vec2 pos, bool is_game) :
  camera_(camera),
  terrain_(terrain),
  effect_(effect),
  position_(pos),
  is_game_(is_game) {
}

void Player::Update() {
  sound_position_.reset();

  const auto input_data = input::Input::GetInstance().GetData();
  direction_face_ = input_data.direction_face;

  ProcessMove();

  if (is_game_) {
    ProcessShift();
    ProcessDigging();
  }
}

void Player::Render() const {
  render::CharaRenderer::Render(camera_, position_, direction_face_, ColorF { 0.01, 0.00, 0.02 }, ColorF { 0.2, 0.9, 0.3 }, 1.0, 1.0, 1.0);

  const auto& lightbloom = render::LightBloom::GetInstance();

  if (is_game_) {
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
      const Vec2 default_dig_pos = position_ + direction_face_ * kDigRange;
      Line { position_ + direction_face_ * 0.5, default_dig_pos }.draw(LineStyle::RoundDot, 0.10, ColorF { 0.25, 0.25, 0.25 });
    }
  }
}

void Player::ProcessMove() {
  const auto input_data = input::Input::GetInstance().GetData();

  const double speed = 7.0;
  if (not input_data.shift) {
    position_ += input_data.direction_move.normalized() * speed * Scene::DeltaTime();
  }
  position_ = terrain_.PushbackService(Circle { position_, kCharacterRadius });
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
  digging_position_ = terrain_.CalcLineCollisionPoint(position_, direction_face_, kDigRange);

  if (input_data.dig && digging_position_ && dig_timer_ <= 0.0) {
    terrain_.DigAt(*digging_position_, 2.0, 0.15, 0.01);
    dig_timer_ = 0.2;
    effect_.add<effect::Dig>(camera_, *digging_position_, direction_face_);
    sound_position_ = (*digging_position_ * 0.2 + position_ * 0.8);

    if (terrain_.GetEarnedSinhalite() > 0) {
      effect_.add<effect::DigSinhalite>(camera_, *digging_position_, direction_face_);
    }
  }
}

}
