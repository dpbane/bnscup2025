#include "stdafx.h"
#include "dig_sinhalite.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::effect {

DigSinhalite::DigSinhalite(const camera::Camera& cam, const Vec2& position, const Vec2& direction) :
  camera_(cam) {
  const int particle_count = 100;
  particles_.reserve(particle_count);
  for (int k = 0; k < particle_count; ++k) {
    const double speed = Random(20.0, 40.0);
    const double angle = (-direction).getAngle() + Random(-90.0_deg, 90.0_deg);
    Particle p {
      .previous_position = position,
      .position = position,
      .velocity = Vec2 { Sin(angle), -Cos(angle) } *speed,
    };
    particles_.push_back(p);
  }

}

bool DigSinhalite::update(double t) {
  bool ret { false };

  for (auto& p : particles_) {
    p.previous_position = p.position;
    p.position += p.velocity * Scene::DeltaTime();
    p.velocity *= Pow(0.2, Scene::DeltaTime());
  }

  {
    const auto target = render::LightBloom::GetInstance().CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto transformer = camera_.CreateRenderTransformer();

    for (const auto& p : particles_) {
      if (p.velocity.lengthSq() < 10.0 * 10.0) continue;
      Line { p.previous_position, p.position }.draw(LineStyle::RoundCap, 0.60, ColorF { 0.5, 0.4, 0.08 });
      ret = true;
    }
  }
  render::LightBloom::GetInstance().Apply(1.0, 10.0, 50.0);

  rect_alpha *= Pow(0.1, Scene::DeltaTime() * 4);
  Scene::Rect().draw(ColorF { 1.0, 0.98, 0.6, rect_alpha });


  return ret;
}


}
