#include "stdafx.h"
#include "selector.hpp"

#include "input/input.hpp"
#include "player/grade_value_converter.hpp"

namespace bnscup2025::player {

Selector::Selector(const PowerGrade& power_grade) :
  power_grade_(power_grade) {
}

void Selector::Update() {
  const auto& input_data = input::Input::GetInstance().GetData();

  UpdateItem();

  // 右上方向切替
  if (input_data.action_change_upright && not input_data.action_change_downleft) {
    selected_item_ = left_item_;
    disp_offset_ = 1.0;
    UpdateItem();
  };
  // 左下方向切替
  if (input_data.action_change_downleft && not input_data.action_change_upright) {
    selected_item_ = right_item_;
    disp_offset_ = -1.0;
    UpdateItem();
  }

  disp_offset_ *= Pow(0.05, Scene::DeltaTime() * 10);
}

void Selector::Render() const {
  if (not selected_item_) return;

  const Vec2 offset_pos = Vec2 { -Scene::Width(), Scene::Height() }.normalized() * disp_offset_;

  // 色ズレエフェクト描画ラムダ
  auto DrawWithColorOffset = [&](const String& text, double size, const Vec2& center_pos, double main_offset_rate, double color_offset_rate) {
    const ScopedRenderStates2D blend { BlendState::Additive };
    FontAsset(U"Text")(text).drawAt(size, center_pos + offset_pos * (main_offset_rate + color_offset_rate), ColorF { 1, 0, 0 });
    FontAsset(U"Text")(text).drawAt(size, center_pos + offset_pos * (main_offset_rate + 0), ColorF { 0, 1, 0 });
    FontAsset(U"Text")(text).drawAt(size, center_pos + offset_pos * (main_offset_rate - color_offset_rate), ColorF { 0, 0, 1 });
  };

  // メイン文字
  const double main_font_size = double(Scene::Height()) / 1080.0 * 64.0;
  DrawWithColorOffset(
    GetItemName(*selected_item_),
    main_font_size,
    Vec2(Scene::Width() * 0.90, Scene::Height() * 0.75),
    Scene::Height() * 0.10,
    Scene::Height() * 0.010
  );

  // サブ文字
  const double sub_font_size = double(Scene::Height()) / 1080.0 * 36.0;
  if (right_item_) {
    DrawWithColorOffset(
      GetItemName(*right_item_),
      sub_font_size,
      Vec2(Scene::Width() * 0.95, Scene::Height() * 0.70),
      Scene::Height() * 0.060,
      Scene::Height() * 0.0050
    );
  }
  if (left_item_) {
    DrawWithColorOffset(
      GetItemName(*left_item_),
      sub_font_size,
      Vec2(Scene::Width() * 0.85, Scene::Height() * 0.80),
      Scene::Height() * 0.060,
      Scene::Height() * 0.0050
    );
  }

}

void Selector::UpdateItem() {
  FindItem();
  FindNextItem(true);
  FindNextItem(false);
}

void Selector::FindItem() {
  if (selected_item_) return;

  if (gvc_.HasNobiru()) {
    selected_item_.emplace(PowerGradeItem::Nobiru);
    return;
  }
  if (gvc_.HasKokoro()) {
    selected_item_.emplace(PowerGradeItem::Kokoro);
    return;
  }
  if (gvc_.HasSusumu()) {
    selected_item_.emplace(PowerGradeItem::Susumu);
    return;
  }
  if (gvc_.HasTsutsu()) {
    selected_item_.emplace(PowerGradeItem::Tsutsu);
    return;
  }
}

void Selector::FindNextItem(bool to_right) {
  if (not selected_item_) return;

  Optional<PowerGradeItem>& target_item = to_right ? right_item_ : left_item_;

  constexpr PowerGradeItem candidates[] = {
    PowerGradeItem::Nobiru,
    PowerGradeItem::Kokoro,
    PowerGradeItem::Susumu,
    PowerGradeItem::Tsutsu,
  };
  // 4つの数値が連番になっていることを利用するロジック。連番でない場合は破綻するため注意。
  const size_t selected_index = static_cast<size_t>(*selected_item_) - static_cast<size_t>(PowerGradeItem::Nobiru);
  for (size_t k = 0; k < 3; ++k) {
    const size_t index = (selected_index + (to_right ? 1 + k : 3 - k)) % 4;
    const PowerGradeItem candidate = candidates[index];
    switch (candidate) {
      case PowerGradeItem::Nobiru:
        if (gvc_.HasNobiru()) {
          target_item.emplace(candidate);
          return;
        }
        break;
      case PowerGradeItem::Kokoro:
        if (gvc_.HasKokoro()) {
          target_item.emplace(candidate);
          return;
        }
        break;
      case PowerGradeItem::Susumu:
        if (gvc_.HasSusumu()) {
          target_item.emplace(candidate);
          return;
        }
        break;
      case PowerGradeItem::Tsutsu:
        if (gvc_.HasTsutsu()) {
          target_item.emplace(candidate);
          return;
        }
        break;
    }
  }
}

String Selector::GetItemName(PowerGradeItem item) const {
  switch (item) {
    case PowerGradeItem::Nobiru: return U"伸";
    case PowerGradeItem::Kokoro: return U"心";
    case PowerGradeItem::Susumu: return U"進";
    case PowerGradeItem::Tsutsu: return U"慎";
  }
  return U"";
}


}
