#pragma once

#include "camera/camera.hpp"
#include "player/player.hpp"

namespace bnscup2025::npc {

class Shop {
public:
  Shop(const camera::Camera& camera, const player::Player& player, const Vec2& position);

  void Update();
  void Render() const;

private:
  const camera::Camera& camera_;
  const player::Player& player_;

  const Vec2 position_;


};

}
