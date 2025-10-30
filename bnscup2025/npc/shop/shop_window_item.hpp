#pragma once

#include "player/power_grade.hpp"

namespace bnscup2025::npc {

class ShopWindowItem {
public:


public:
  ShopWindowItem(const player::PowerGrade& power_grade, const int& sinhalite_amount, const Vec2& position, const Vec2& direction, player::PowerGradeItem pg_item);

  void Update();
  void Render() const;

  void SetActive(bool active) { is_active_ = active; }

  player::PowerGradeItem GetPowerGradeItem() const { return pg_item_; }
  bool JustBought() const { return just_bought_; }

  double GetAlpha() const { return alpha_; }

private:
  void UpdateActive();
  void UpdateFocus();
  void UpdateBuy();

  bool CanBuy() const;

private:
  const player::PowerGrade& power_grade_;
  const player::PowerGradeItem pg_item_;
  const int& sinhalite_amount_;
  const Vec2 position_;
  const Vec2 direction_;

  bool is_active_ { false };
  bool selected_ { false };
  double alpha_ { 0.0 };
  double focus_ { 0.0 };
  double buy_timer_ { 0.0 };

  bool just_bought_ { false };

};

}
