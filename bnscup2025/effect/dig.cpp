#include "stdafx.h"
#include "dig.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::effect {

Dig::Dig(const camera::Camera& cam, const Vec2& position, const Vec2& direction) :
  camera_(cam) {

  const int particle_count = 10;
  particles_.reserve(particle_count);
  for (int k = 0; k < particle_count; ++k) {
    const double speed = Random(15.0, 30.0);
    const double angle = (-direction).getAngle() + Random(-90.0_deg, 90.0_deg);
    Particle p {
      .previous_position = position,
      .position = position,
      .velocity = Vec2 { Sin(angle), -Cos(angle) } *speed,
    };
    particles_.push_back(p);
  }
}

bool Dig::update(double t) {
  bool ret { false };

  for (auto& p : particles_) {
    p.previous_position = p.position;
    p.position += p.velocity * Scene::DeltaTime();
    p.velocity *= Pow(0.01, Scene::DeltaTime() * 2);
  }

  {
    const auto target = render::LightBloom::GetInstance().CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto transformer = camera_.CreateRenderTransformer();

    for (const auto& p : particles_) {
      if (p.velocity.lengthSq() < 4.0 * 4.0) continue;
      Line { p.previous_position, p.position }.draw(0.10, ColorF { 0.2, 0.25, 0.4 });
      ret = true;
    }
  }
  render::LightBloom::GetInstance().Apply(5.0, 10.0, 50.0);


  return ret;
}

}
