#include "stdafx.h"
#include "shop.hpp"

namespace bnscup2025::npc {

Shop::Shop(const camera::Camera& camera, const player::Player& player, const Vec2& position) :
  camera_(camera),
  player_(player),
  position_(position) {
}



}
