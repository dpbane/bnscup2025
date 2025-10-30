#include "stdafx.h"
#include "sinhalite_disp.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::ui {

SinhaliteText::SinhaliteText(const int& sinhalite_amount) :
  sinhalite_amount_(sinhalite_amount) {
}

void SinhaliteText::Update() {
  const auto& fade = screen::Fade::GetInstance();
  if (fade.IsFadingIn()) alpha_ = 0.0;
  if (fade.CompletedFadeIn()) alpha_ += Scene::DeltaTime() * 5.0;
  if (fade.IsFadingOut()) alpha_ -= Scene::DeltaTime() * 5.0;
  alpha_ = Clamp(alpha_, 0.0, 1.0);
}

void SinhaliteText::Render() const {
  const double font_size = double(Scene::Height()) / 1080 * 38;
  FontAsset(U"Text")(U"シンハライト: {:}"_fmt(sinhalite_amount_))
    .draw(
      font_size,
      Vec2 { Scene::Width() * 0.01, Scene::Height() * 0.80 },
      ColorF { 1.0, 1.0, 0.5, alpha_ }
    );
}

}
