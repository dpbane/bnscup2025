#include "stdafx.h"
#include "shop_window.hpp"

#include "item_info_factory.hpp"

namespace bnscup2025::npc {

ShopWindow::ShopWindow(player::PowerGrade& power_grade, int& sinhalite, const player::Player& player) :
  power_grade_(power_grade),
  sinhalite_(sinhalite),
  player_(player) {

  ReRollItems();
}

void ShopWindow::Update() {
  for (auto& item : items_) {
    if (item) {
      item->Update();
      if (item->JustBought()) {
        const int cost = ItemInfoFactory::Get(item->GetPowerGradeItem()).costs[power_grade_[item->GetPowerGradeItem()]];
        if (item->GetPowerGradeItem() == player::PowerGradeItem::Reroll) {
          ReRollItems();
        }
        else {
          power_grade_[item->GetPowerGradeItem()] += 1;
        }
        sinhalite_ -= cost;
      }
    }
  }
}

void ShopWindow::Render() const {
  for (const auto& item : items_) {
    item->Render();
  }

  // 購入説明表示
  FontAsset(U"Text")(U"買いたいもののほうを向いて、左右クリック/LB/RB長押しで購入")
    .drawAt(Scene::Height() * 0.05, Vec2 { Scene::Center().x, Scene::Height() * 0.035 }, ColorF { 1.0 }.withA(items_[0]->GetAlpha()));
}

void ShopWindow::SetActive(bool active) {
  for (auto& item : items_) {
    item->SetActive(active);
  }
}

void ShopWindow::ReRollItems() {
  const auto selected_items = SelectRandomItems();
  items_[0].emplace(power_grade_, sinhalite_, Vec2 { Scene::Width() * 0.25, Scene::Height() * 0.50 }, Vec2 { -1.0, 0.0 }, selected_items[0], player_);
  items_[1].emplace(power_grade_, sinhalite_, Vec2 { Scene::Width() * 0.50, Scene::Height() * 0.25 }, Vec2 { 0.0, -1.0 }, selected_items[1], player_);
  items_[2].emplace(power_grade_, sinhalite_, Vec2 { Scene::Width() * 0.75, Scene::Height() * 0.50 }, Vec2 { 1.0, 0.0 }, selected_items[2], player_);
  items_[3].emplace(power_grade_, sinhalite_, Vec2 { Scene::Width() * 0.50, Scene::Height() * 0.75 }, Vec2 { 0.0, 1.0 }, selected_items[3], player_);
}

std::array<player::PowerGradeItem, 4> ShopWindow::SelectRandomItems() const {
  Array<player::PowerGradeItem> all_items;
  Array<player::PowerGradeItem> candidate;
  for (int k = 0; k < std::to_underlying<player::PowerGradeItem>(player::PowerGradeItem::Reroll); ++k) {
    const auto item = static_cast<player::PowerGradeItem>(k);
    all_items.push_back(item);
    if (power_grade_[item] >= ItemInfoFactory::Get(item).costs.size()) continue;
    candidate.push_back(item);
  };

  Array<player::PowerGradeItem> choosen_items;
  if (candidate.size() >= 3) {
    choosen_items = candidate.choice(3);
  }
  else {
    choosen_items = candidate;
    const auto needed_items = 3 - candidate.size();
    Array<player::PowerGradeItem> rest_items;
    for (const auto& item : all_items) {
      if (not candidate.contains(item)) {
        rest_items.push_back(item);
      }
    }
    const auto added_items = rest_items.choice(needed_items);
    for (const auto& item : added_items) {
      choosen_items.push_back(item);
    }
  }

  return std::array<player::PowerGradeItem, 4> {
    choosen_items[0],
      choosen_items[1],
      choosen_items[2],
      player::PowerGradeItem::Reroll
  };
}




}
