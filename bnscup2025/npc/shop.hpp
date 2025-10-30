#pragma once

#include "camera/camera.hpp"
#include "player/player.hpp"

#include "shop/shop_window.hpp"

namespace bnscup2025::npc {

class Shop {
public:
  Shop(const camera::Camera& camera, const player::Player& player, player::PowerGrade& power_grade, const Vec2& position, int& sinhalite_amount);

  void Update();
  void Render() const;

private:
  const camera::Camera& camera_;
  const player::Player& player_;

  const Vec2 position_;
  ShopWindow shop_window_;



};

}
