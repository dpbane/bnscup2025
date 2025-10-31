#include "stdafx.h"
#include "shop.hpp"

#include "input/input.hpp"
#include "render/chara_renderer.hpp"

namespace bnscup2025::npc {

constexpr double kSpeakDistance = 4.0;

Shop::Shop(const camera::Camera& camera, const player::Player& player, player::PowerGrade& power_grade, const Vec2& position, int& sinhalite_amount) :
  camera_(camera),
  player_(player),
  position_(position),
  shop_window_ { power_grade, sinhalite_amount, player } {
}

void Shop::Update() {
  const auto input_data = input::Input::GetInstance().GetData();
  const Vec2 player_pos = player_.GetPosition();
  const double distance = (player_pos - position_).length();

  shop_window_.SetActive(distance <= (kSpeakDistance));
  shop_window_.Update();
}

void Shop::Render() const {
  const Vec2 direction = (player_.GetPosition() - position_).normalized();
  ColorF color_body { 0.1, 0.06, 0.05 };
  ColorF color_edge { 0.72, 0.45, 0.34 };
  render::CharaRenderer::Render(camera_, position_, direction, color_body, color_edge, 1.0, 1.0, 1.0);

  shop_window_.Render();
}


}
