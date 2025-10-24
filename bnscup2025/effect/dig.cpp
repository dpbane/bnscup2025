#include "stdafx.h"
#include "dig.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::effect {

Dig::Dig(const camera::Camera& cam, const Vec2& position, const Vec2& direction, const ColorF& color) :
  camera_(cam),
  color_(color) {

  const int particle_count = 10;
  particles_.reserve(particle_count);
  for (int k = 0; k < particle_count; ++k) {
    const double speed = Random(8.0, 16.0);
    const double angle = (-direction).getAngle() + Random(-90.0_deg, 90.0_deg);
    Particle p {
      .position = position,
      .velocity = Vec2 { Sin(angle), -Cos(angle) } *speed,
      .radius = Random(0.4, 0.7)
    };
    particles_.push_back(p);
  }
}

bool Dig::update(double t) {
  bool ret { false };

  for (auto& p : particles_) {
    p.position += p.velocity * Scene::DeltaTime();
    p.radius *= Pow(0.1, Scene::DeltaTime() * 8);
  }

  {
    const auto target = render::LightBloom::GetInstance().CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto transformer = camera_.CreateRenderTransformer();

    for (const auto& p : particles_) {
      if (p.radius < 0.001) continue;
      Circle { p.position, p.radius }.draw(color_);
      ret = true;
    }
  }
  render::LightBloom::GetInstance().Apply(1.0, 1.0, 1.0);


  return ret;
}

}
