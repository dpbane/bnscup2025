#include "stdafx.h"
#include "exit.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::exit {

constexpr double kExitRadius = 1.5;

Exit::Exit(const camera::Camera& camera, const player::Player& player, const Vec2& position) :
  camera_(camera),
  player_(player),
  position_(position) {
}

void Exit::Update() {
  if (IsPlayerOnExit()) {
    timer_ += Scene::DeltaTime();
  }
  else {
    timer_ = 0.0;
  }
}

void Exit::Render() const {
  const auto& lightbloom = render::LightBloom::GetInstance();
  const ColorF color_edge { 0.9, 0.9, 0.2 };
  const ColorF color_body { 0.2, 0.2, 0.05 };
  // 本体
  {
    const auto target = lightbloom.CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto t = camera_.CreateRenderTransformer();
    Circle { position_, kExitRadius }
      .draw(color_body)
      .drawFrame(0.10, color_edge);
  }
  lightbloom.Apply(3.0, 3.0, 3.0);
}

bool Exit::IsPlayerOnExit() const {
  const Vec2 player_pos = player_.GetPosition();
  return (player_pos - position_).lengthSq() <= (kExitRadius * kExitRadius);
}

double Exit::GetPlayerOnExitTime() const {
  return timer_;
}

bool Exit::ShouldExitGame() const {
  constexpr double kExitTimeThreshold = 2.0;
  return timer_ >= kExitTimeThreshold;
}



}
