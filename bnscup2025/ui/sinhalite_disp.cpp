#include "stdafx.h"
#include "sinhalite_disp.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::ui {

SinhaliteText::SinhaliteText(const int& sinhalite_amount, bool visible) :
  sinhalite_amount_(sinhalite_amount),
  last_amount_(sinhalite_amount),
  visible_(visible) {
}

void SinhaliteText::Update() {
  const auto& fade = screen::Fade::GetInstance();
  if (fade.IsFadingIn()) alpha_ = 0.0;
  if (fade.CompletedFadeIn()) alpha_ += Scene::DeltaTime() * 5.0;
  if (fade.IsFadingOut()) alpha_ -= Scene::DeltaTime() * 5.0;
  alpha_ = Clamp(alpha_, 0.0, 1.0);

  if (sinhalite_amount_ > last_amount_) {
    plus_amount_ += (sinhalite_amount_ - last_amount_);
    plus_timer_ = 1.0;
  }

  if (plus_timer_ > 0.0) {
    plus_timer_ -= Scene::DeltaTime();
  }
  if (plus_timer_ <= 0.0) {
    plus_timer_ = 0.0;
    plus_amount_ = 0;
  }

  last_amount_ = sinhalite_amount_;
}

void SinhaliteText::Render() const {
  if (not visible_) return;
  const double font_size = double(Scene::Height()) / 1080 * 38;
  const Vec2 pos { Scene::Width() * 0.01, Scene::Height() * 0.80 };
  FontAsset(U"Text")(U"シンハライト: {:}"_fmt(sinhalite_amount_))
    .draw(
      font_size,
      pos,
      ColorF { 1.0, 1.0, 0.5, alpha_ }
    );

  if (plus_amount_ > 0) {
    const double shift_x = Scene::Height() / 1080.0 * 200.0;
    const double shift_y = -EaseOutExpo((1.0 - plus_timer_)) * Scene::Height() / 1080 * 100;
    FontAsset(U"Text")(U"+{:}"_fmt(plus_amount_))
      .draw(
        font_size * 1.5,
        pos.movedBy(shift_x, shift_y),
        ColorF { 1.0, 1.0, 0.2, alpha_ }
      );
  }
}

}
