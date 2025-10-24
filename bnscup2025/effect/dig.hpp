#pragma once

#include "camera/camera.hpp"

namespace bnscup2025::effect {

class Dig : public IEffect {
private:
  struct Particle {
    Vec2 position;
    Vec2 velocity;
    double radius;
  };

public:
  explicit Dig(const camera::Camera& cam, const Vec2& position, const Vec2& direction, const ColorF& color);

  bool update(double t) override;

private:
  const camera::Camera& camera_;
  Array<Particle> particles_;
  const ColorF color_;

};

}
