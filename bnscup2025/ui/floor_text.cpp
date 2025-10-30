#include "stdafx.h"
#include "floor_text.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::ui {

FloorText::FloorText(const String& text) :
  text_(text) {
}

void FloorText::Update() {
  if (not screen::Fade::GetInstance().CompletedFadeIn()) return;
  timer_ += Scene::DeltaTime();
}

void FloorText::Render() const {
  double alpha = 0.0;

  if (timer_ < 0.5) {
    alpha = (timer_ / 0.5);
  }
  else if (timer_ < 2.5) {
    alpha = 1.0;
  }
  else if (timer_ < 3.0) {
    alpha = 1.0 - ((timer_ - 2.5) / 0.5);
  }

  const double font_size = double(Scene::Height()) / 1080 * 64;
  FontAsset(U"Text")(text_)
    .drawAt(font_size, Scene::Center().movedBy(0, Scene::Height() / 3), ColorF { 1.0, 1.0, 1.0, alpha });
}

}
