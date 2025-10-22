#include "stdafx.h"
#include "player.hpp"

namespace bnscup2025::player {

Player::Player(Vec2 pos) :
  position_(pos) {
}

void Player::Update() {
}

void Player::Render(const camera::Camera& cam) const {
  const auto t = cam.CreateRenderTransformer();

  Circle { position_, cam.GetCellSize().x * 0.40 }.drawFrame(0.10, ColorF { 0.2, 1.0, 0.3 });

}

}
