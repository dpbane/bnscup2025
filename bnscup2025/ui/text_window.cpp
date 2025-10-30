#include "stdafx.h"
#include "text_window.hpp"


namespace bnscup2025::ui {

TextWindow::TextWindow() {
}

void TextWindow::SetText(const String& text) {
  if (text != current_text_) {
    current_text_ = text;
    text_alpha_ = 0.0;
  }
}

void TextWindow::SetActive() {
  is_active_ = true;
}

void TextWindow::Reset() {
  current_text_.clear();
  text_window_alpha_ = 0.0;
  text_alpha_ = 0.0;
  is_active_ = false;
}

void TextWindow::Update() {
  if (is_active_) {
    text_window_alpha_ += Scene::DeltaTime() * 3.0;
    text_alpha_ += Scene::DeltaTime() * 3.0;
  }
  else {
    text_window_alpha_ -= Scene::DeltaTime() * 2.0;
    text_alpha_ -= Scene::DeltaTime() * 3.0;
  }
  text_window_alpha_ = Clamp(text_window_alpha_, 0.0, 1.0);
  text_alpha_ = Clamp(text_alpha_, 0.0, 1.0);

  is_active_ = false;
}

void TextWindow::Render() const {
  Rect window { Point{0, Scene::Height() / 8}, Size{Scene::Width(), Scene::Height() / 4} };
  window.draw(ColorF { 0, 0, 0, text_window_alpha_ * 0.8 });
  const double font_size = double(Scene::Height()) / 1080.0 * 64;
  FontAsset(U"Text")(current_text_).drawAt(font_size, window.center(), ColorF { 1, 1, 1, text_alpha_ });
}


}
