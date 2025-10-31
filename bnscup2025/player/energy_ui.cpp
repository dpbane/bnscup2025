#include "stdafx.h"
#include "energy_ui.hpp"

namespace bnscup2025::player {

EnergyUI::EnergyUI(const PowerGrade& power_grade, const double& energy) :
  gvc_(power_grade),
  energy_(energy) {
}

void EnergyUI::Update() {
  // 満タン時のまま1秒経過したかを判定
  if (energy_ >= gvc_.GetMaxEnergy()) {
    alpha_timer_ += Scene::DeltaTime();
  }
  else {
    alpha_timer_ -= Scene::DeltaTime();
  }
  alpha_timer_ = Clamp(alpha_timer_, 0.0, 1.0);

  // 透明度を更新
  if (alpha_timer_ == 1.0) {
    alpha_ -= Scene::DeltaTime() * 2.0;
  }
  else {
    alpha_ += Scene::DeltaTime() * 2.0;
  }
  alpha_ = Clamp(alpha_, 0.0, 1.0);
}

void EnergyUI::Render(Optional<double> burnout_rate, double radius) const {
  const double max_inner_rate = Min(1.0, gvc_.GetMaxEnergy() / 10000.0);
  const double max_outer_rate = Max((gvc_.GetMaxEnergy() - 10000.0) / 10000.0, 0.0);
  const double inner_thickness = Scene::Height() / 1080.0 * 10.0;
  const double outer_thickness = inner_thickness * 0.5;
  const ColorF background_color { 0.5, 0.5, 0.5, alpha_ };
  const ColorF foreground_color { 0.0, 1.0, 1.0, alpha_ };
  const ColorF burnout_color { 1.0, 1.0, 1.0, alpha_ };

  Circle inner { Scene::CenterF(), radius };
  Circle outer { Scene::CenterF(), radius + inner_thickness * 1.5 };

  if (not burnout_rate) {
    // 通常のエネルギー描画

    // まずは最大値描画
    inner.drawArc(0_deg, -360_deg * max_inner_rate, 0, inner_thickness, background_color);
    if (max_outer_rate > 0.0) {
      outer.drawArc(0_deg, -360_deg * max_outer_rate, 0, outer_thickness, background_color);
    }

    // 次に現在値描画
    const double inner_rate = Min(1.0, energy_ / 10000.0);
    const double outer_rate = Max((energy_ - 10000.0) / 10000.0, 0.0);
    inner.drawArc(0_deg, -360_deg * inner_rate, 0, inner_thickness, foreground_color);
    if (outer_rate > 0.0) {
      outer.drawArc(0_deg, -360_deg * outer_rate, 0, outer_thickness, foreground_color);
    }
  }
  else {
    // バーンアウト時のエネルギー描画
    inner.drawArc(0_deg, -360_deg, 0, inner_thickness, background_color);
    inner.drawArc(0_deg, -360_deg * (1.0 - *burnout_rate), 0, inner_thickness, burnout_color);
  }


}

}
