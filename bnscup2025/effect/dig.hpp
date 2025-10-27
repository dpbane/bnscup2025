#pragma once

#include "camera/camera.hpp"

namespace bnscup2025::effect {

/// @brief 壁を採掘したときのエフェクト。
class Dig : public IEffect {
private:
  struct Particle {
    Vec2 previous_position;
    Vec2 position;
    Vec2 velocity;
  };

public:
  explicit Dig(const camera::Camera& cam, const Vec2& position, const Vec2& direction);

  bool update(double t) override;

private:
  const camera::Camera& camera_;
  Array<Particle> particles_;

};

}
