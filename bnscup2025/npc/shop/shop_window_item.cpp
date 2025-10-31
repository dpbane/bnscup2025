#include "stdafx.h"
#include "shop_window_item.hpp"

#include "input/input.hpp"
#include "item_info_factory.hpp"

namespace bnscup2025::npc {

ShopWindowItem::ShopWindowItem(const player::PowerGrade& power_grade,
                               const int& sinhalite_amount,
                               const Vec2& position,
                               const Vec2& direction,
                               player::PowerGradeItem pg_item,
                               const player::Player& player) :
  power_grade_(power_grade),
  pg_item_(pg_item),
  sinhalite_amount_(sinhalite_amount),
  position_(position),
  direction_(direction),
  player_(player) {
}

void ShopWindowItem::Update() {
  UpdateActive();
  UpdateFocus();
  UpdateBuy();
}

void ShopWindowItem::Render() const {
  SizeF size = Scene::Size() * 0.2;
  RectF rect{ position_ - size / 2, size };

  // 枠表示
  rect.draw(ColorF{ 0.2, 0.2, 0.2, alpha_ * 1.0 });
  if (selected_) {
    rect.drawFrame(0.0, size.y * 0.02, ColorF{ 0.5, 0.8, 0.9, alpha_ });
  }


  const ItemInfo item_info = ItemInfoFactory::Get(pg_item_);

  // 名前表示
  FontAsset(U"Text")(item_info.name)
    .drawAt(
      rect.h * 0.2,
      rect.center().movedBy(0, size.y * -0.25),
      ColorF{ 1.0, 1.0, 1.0, alpha_ }
    );

  // レベル表示
  String str_level =
    (pg_item_ == player::PowerGradeItem::Reroll) ? U""
    : (power_grade_[pg_item_] >= item_info.costs.size())
    ? U"Lv. MAX"
    : U"Lv. {:} → {:}"_fmt(power_grade_[pg_item_], power_grade_[pg_item_] + 1);
  ColorF color_level =
    (power_grade_[pg_item_] >= item_info.costs.size()) ? ColorF{ 1.0, 0.2, 0.2 } : ColorF{ 1.0, 0.9, 0.5 };
  FontAsset(U"Text")(str_level)
    .drawAt(
      rect.h * 0.15,
      rect.center().movedBy(0, size.y * 0.05),
      color_level.withA(alpha_)
    );

  // コスト表示
  String str_cost =
    (power_grade_[pg_item_] >= item_info.costs.size()) ? U""
    : U"必要シンハライト: {:}"_fmt(item_info.costs[power_grade_[pg_item_]]);
  ColorF color_cost = CanBuy() ? ColorF{ 0.5, 1.0, 0.3 } : ColorF{ 1.0, 0.2, 0.2 };
  FontAsset(U"Text")(str_cost)
    .drawAt(
      rect.h * 0.15,
      rect.center().movedBy(0, size.y * 0.25),
      color_cost.withA(alpha_)
    );

  // 説明文表示
  if (selected_) {
    FontAsset(U"Text")(item_info.description)
      .drawAt(Scene::Height() * 0.035, Vec2{ Scene::Center().x, Scene::Height() * (1.0 - 0.070) }, ColorF{ 1.0 }.withA(alpha_));
  }

  // 購入バー表示
  if (buy_timer_ > 0.0) {
    const Vec2 tl{ rect.leftX(), rect.topY() };
    const Vec2 tr{ rect.rightX(), rect.topY() };
    const Vec2 bl{ rect.leftX(), rect.bottomY() };
    const Vec2 br{ rect.rightX(), rect.bottomY() };
    const Line top{ tl, tl.lerp(tr, EaseInOutQuint(buy_timer_)) };
    const Line right{ tr, tr.lerp(br, EaseInOutQuint(buy_timer_)) };
    const Line bottom{ br, br.lerp(bl, EaseInOutQuint(buy_timer_)) };
    const Line left{ bl, bl.lerp(tl, EaseInOutQuint(buy_timer_)) };
    top.draw(size.y * 0.03, ColorF{ 0.5, 1.0, 0.3, alpha_ });
    right.draw(size.y * 0.03, ColorF{ 0.5, 1.0, 0.3, alpha_ });
    bottom.draw(size.y * 0.03, ColorF{ 0.5, 1.0, 0.3, alpha_ });
    left.draw(size.y * 0.03, ColorF{ 0.5, 1.0, 0.3, alpha_ });
  }
}

void ShopWindowItem::UpdateActive() {
  if (is_active_) {
    alpha_ += Scene::DeltaTime() * 5.0;
  }
  else {
    alpha_ -= Scene::DeltaTime() * 5.0;
  }
  alpha_ = Clamp(alpha_, 0.0, 1.0);
}

void ShopWindowItem::UpdateFocus() {
  const double dot_product = direction_.dot(player_.GetDirectionFace());
  selected_ = (dot_product > 0.75);  // 0.75 > 1/sqrt(2) = cos(45deg)

  if (selected_) {
    focus_ += Scene::DeltaTime() * 5.0;
  }
  else {
    focus_ -= Scene::DeltaTime() * 5.0;
  }
  focus_ = Clamp(focus_, 0.0, 1.0);
}

void ShopWindowItem::UpdateBuy() {
  const auto input_data = input::Input::GetInstance().GetData();

  just_bought_ = false;
  if (not selected_ || not is_active_ || not CanBuy()) {
    buy_timer_ = 0.0;
    return;
  }

  const double pre_buy_timer = buy_timer_;
  if ((input_data.confirm_trigger && buy_timer_ == 0.0) ||
      (input_data.confirm_keep && buy_timer_ > 0.0)) {
    buy_timer_ += Scene::DeltaTime();
  }
  else {
    buy_timer_ = 0.0;
  }

  if (pre_buy_timer < 1.0 && buy_timer_ >= 1.0) {
    just_bought_ = true;
    buy_timer_ = 0.0;
  }
  buy_timer_ = Clamp(buy_timer_, 0.0, 1.0);
}

bool ShopWindowItem::CanBuy() const {
  const ItemInfo item_info = ItemInfoFactory::Get(pg_item_);
  const int cost = item_info.costs[power_grade_[pg_item_]];
  if (power_grade_[pg_item_] >= item_info.costs.size()) return false;
  if (sinhalite_amount_ < cost) return false;
  return true;
}

}
