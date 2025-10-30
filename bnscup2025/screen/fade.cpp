#include "stdafx.h"
#include "fade.hpp"

namespace bnscup2025::screen {

Fade::Fade() {
}

Fade& Fade::GetInstance() {
  static Fade instance;
  return instance;
}

void Fade::Update() {
  if (is_fading_in_) {
    alpha_ -= Scene::DeltaTime() / fade_duration_;
    if (alpha_ <= 0.0) {
      alpha_ = 0.0;
      is_fading_in_ = false;
    }
  }
  if (is_fading_out_) {
    alpha_ += Scene::DeltaTime() / fade_duration_;
    if (alpha_ >= 1.0) {
      alpha_ = 1.0;
      is_fading_out_ = false;
    }
  }
}

void Fade::Render() const {
  Scene::Rect().draw(ColorF { 0.0, alpha_ });
}

void Fade::BeginFadeIn(double fade_duration) {
  is_fading_in_ = true;
  is_fading_out_ = false;
  fade_duration_ = fade_duration;
  alpha_ = 1.0;
}

void Fade::BeginFadeOut(double fade_duration) {
  is_fading_in_ = false;
  is_fading_out_ = true;
  fade_duration_ = fade_duration;
  alpha_ = 0.0;
}

bool Fade::CompletedFadeIn() const {
  return
    not is_fading_in_ &&
    not is_fading_out_ &&
    alpha_ <= 0.0;
}

bool Fade::CompletedFadeOut() const {
  return
    not is_fading_in_ &&
    not is_fading_out_ &&
    alpha_ >= 1.0;
}

}
