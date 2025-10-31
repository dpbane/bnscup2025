#pragma once

#include "player/power_grade.hpp"
#include "player/grade_value_converter.hpp"

namespace bnscup2025::player {

class Selector {
public:
  Selector(const PowerGrade& power_grade);

  void Update(bool can_change);
  void Render() const;

  Optional<PowerGradeItem> GetSelectedItem() const { return selected_item_; }

private:
  void UpdateItem();
  void FindItem();
  void FindNextItem(bool to_right);

  String GetItemName(PowerGradeItem item) const;

private:
  const PowerGrade& power_grade_;
  const GradeValueConverter gvc_ { power_grade_ };

  Optional<PowerGradeItem> selected_item_;
  Optional<PowerGradeItem> right_item_;
  Optional<PowerGradeItem> left_item_;

  double disp_offset_ { 0.0 };

};

}
