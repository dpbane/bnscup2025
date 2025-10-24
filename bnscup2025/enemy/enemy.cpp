#include "enemy.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::enemy {

Enemy::Enemy(const camera::Camera& camera,
  const terrain::Terrain& terrain,
  const player::Player& player,
  const Vec2& pos) :
  camera_(camera),
  terrain_(terrain),
  player_(player),
  position_(pos) {
}

void Enemy::Update() {
}

void Enemy::Render() const {
  const auto& lightbloom = render::LightBloom::GetInstance();
  const ColorF color_edge { 0.9, 0.2, 0.3 };
  const ColorF color_body { 0.05, 0.01, 0.02 };

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
  lightbloom.Apply(1.0, 2.0, 2.0);
}

void Enemy::ThreadMain() {
}

}
