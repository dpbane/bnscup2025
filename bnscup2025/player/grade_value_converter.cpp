#include "stdafx.h"
#include "grade_value_converter.hpp"

namespace bnscup2025::player {


double GradeValueConverter::GetMaxEnergy() const {
  uint8 level = power_grade_[PowerGradeItem::MaxEnergy];
  int base = HasMiru() ? 5000 : 10000;
  return base + 500 * level;
}

double GradeValueConverter::GetEnergyRegen() const {
  uint8 level = power_grade_[PowerGradeItem::EnergyRegen];
  double denominator = 50.0 - 8.0 * level;  // 満タンになるまでの秒数
  return 10000.0 / denominator;
}

double GradeValueConverter::GetDigCooldown() const {
  uint8 level = power_grade_[PowerGradeItem::DigSpeed];
  double base = 0.25;
  return base - 0.04 * level;
}

double GradeValueConverter::GetDigRange() const {
  uint8 level = power_grade_[PowerGradeItem::DigMight];
  double base = 2.4;
  return base + 0.8 * level;
}

double GradeValueConverter::GetDigDistance() const {
  uint8 level = power_grade_[PowerGradeItem::DigDistance];
  double base = 8.0;
  return base + 2.4 * level;
}

double GradeValueConverter::GetViewAngle() const {
  uint8 level = power_grade_[PowerGradeItem::ViewAngle];
  constexpr double base = 90_deg;
  return base + 20_deg * level;
}

double GradeValueConverter::GetViewDistance() const {
  uint8 level = power_grade_[PowerGradeItem::ViewDistance];
  double base = 52.0;
  return base + 7.0 * level;
}

double GradeValueConverter::GetMoveSpeedRate() const {
  uint8 level = power_grade_[PowerGradeItem::MoveSpeed];
  double base = 1.0;
  return base + 0.12 * level;
}

double GradeValueConverter::GetNobiruShiftDistance() const {
  uint8 level = power_grade_[PowerGradeItem::Nobiru];
  return 4.0 + 1.8 * level;
}

double GradeValueConverter::GetKokoroBeginCost() const {
  uint8 level = power_grade_[PowerGradeItem::Kokoro];
  return 3400.0 - 340.0 * level;
}

double GradeValueConverter::GetKokoroMaintainCost() const {
  uint8 level = power_grade_[PowerGradeItem::Kokoro];
  return 1300.0 - 130.0 * level;
}

double GradeValueConverter::GetSususmuSpeedRate() const {
  uint8 level = power_grade_[PowerGradeItem::Susumu];
  const double base = 1.4;
  return base + level * 0.20;
}

double GradeValueConverter::GetSusumuCostRate() const {
  return 3500.0;
}

double GradeValueConverter::GetTsutsuStartCost() const {
  uint8 level = power_grade_[PowerGradeItem::Tsutsu];
  return 2500.0 - 250.0 * level;
}

double GradeValueConverter::GetTsutsuMaintainCost() const {
  uint8 level = power_grade_[PowerGradeItem::Tsutsu];
  return 500.0 - 50.0 * level;
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
