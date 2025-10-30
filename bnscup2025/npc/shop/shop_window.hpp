#pragma once

#include "shop_window_item.hpp"

namespace bnscup2025::npc {

class ShopWindow {
public:
  ShopWindow(player::PowerGrade& power_grade, int& sinhalite);

  void Update();
  void Render() const;

  void SetActive(bool active);

private:
  void ReRollItems();
  std::array<player::PowerGradeItem, 4> SelectRandomItems() const;

private:
  player::PowerGrade& power_grade_;
  int& sinhalite_;
  std::array<Optional<ShopWindowItem>, 4> items_;


};

}
