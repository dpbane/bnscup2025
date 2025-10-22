#pragma once

#include "camera/camera.hpp"

namespace bnscup2025::player {

class Player {
public:
  Player(Vec2 pos);

  void Update();
  void Render(const camera::Camera& cam) const;

private:
  Vec2 position_;
  Vec2 direction_ { 0.0, -1.0 };


};

}
