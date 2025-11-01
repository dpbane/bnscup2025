#include "stdafx.h"
#include "tutorial_text.hpp"

namespace bnscup2025::ui {

TutorialText::TutorialText(const player::Player& player) :
  player_(player) {
}

void TutorialText::AddEntry(const Entry& entry) {
  entries_.emplace_back(entry);
}

void TutorialText::Update() {
  const Vec2& player_pos = player_.GetPosition();

  // エリア内にいるかチェック
  bool in_any_area = false;
  for (const auto& entry : entries_) {
    if (entry.area.contains(player_pos)) {
      current_text_ = entry.text;
      in_any_area = true;
      break;
    }
  }

  // アルファ値更新
  if (in_any_area) {
    alpha_ += Scene::DeltaTime();
  }
  else {
    alpha_ -= Scene::DeltaTime();
  }
  alpha_ = Clamp(alpha_, 0.0, 1.0);
}

void TutorialText::Render() const {
  const double font_size = double(Scene::Height()) / 1080 * 54;
  FontAsset(U"Text")(current_text_)
    .drawAt(
      font_size,
      Scene::CenterF().movedBy(0.0, Scene::Height() * 0.4),
      ColorF { 1.0, 1.0, 1.0, alpha_ }
    );
}




}
