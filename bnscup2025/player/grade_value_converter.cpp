#include "stdafx.h"
#include "grade_value_converter.hpp"

namespace bnscup2025::player {


double GradeValueConverter::GetMaxEnergy() const {
  uint8 level = power_grade_[PowerGradeItem::MaxEnergy];
  int base = 10000;
  if (HasMiru()) base = -5000;
  return base + 500 * level;
}

double GradeValueConverter::GetNobiruShiftDistance() const {
  uint8 level = power_grade_[PowerGradeItem::Nobiru] - 1;
  return 1.5 + 0.5 * level;
}

double GradeValueConverter::GetKokoroBeginCost() const {
  uint8 level = power_grade_[PowerGradeItem::Kokoro] - 1;
  return 3400.0 - 300.0 * level;
}

double GradeValueConverter::GetKokoroMaintainCost() const {
  uint8 level = power_grade_[PowerGradeItem::Kokoro] - 1;
  return 3300.0 - 200.0 * level;
}

double GradeValueConverter::GetSususmuSpeed() const {
  uint8 level = power_grade_[PowerGradeItem::Susumu] - 1;
  return 7.5 + 0.75 * level;
}

double GradeValueConverter::GetTsutsuStartCost() const {
  uint8 level = power_grade_[PowerGradeItem::Tsutsu] - 1;
  return 2500.0 - 250.0 * level;
}

double GradeValueConverter::GetTsutsuMaintainCost() const {
  uint8 level = power_grade_[PowerGradeItem::Tsutsu] - 1;
  return 750.0 - 75.0 * level;
}

bool GradeValueConverter::HasNobiru() const {
  return power_grade_[PowerGradeItem::Nobiru] > 0;
}

bool GradeValueConverter::HasKokoro() const {
  return power_grade_[PowerGradeItem::Kokoro] > 0;
}

bool GradeValueConverter::HasSusumu() const {
  return power_grade_[PowerGradeItem::Susumu] > 0;
}

bool GradeValueConverter::HasTsutsu() const {
  return power_grade_[PowerGradeItem::Tsutsu] > 0;
}

bool GradeValueConverter::HasMiru() const {
  return power_grade_[PowerGradeItem::Miru] > 0;
}

bool GradeValueConverter::HasGod() const {
  return power_grade_[PowerGradeItem::God] > 0;
}

}
