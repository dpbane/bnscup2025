#pragma once

#include "power_grade.hpp"

namespace bnscup2025::player {

class GradeValueConverter {
public:
  GradeValueConverter(const PowerGrade& power_grade) :
    power_grade_(power_grade) {
  }

  int GetMaxEnergy() const { return power_grade_.max_energy; }
  double GetRegenerateRate() const;
  double GetDiggingDuration() const;
  double GetHariCost() const;
  double GetSusumuBeginCost() const;
  double GetSususmuMaintainCost() const;
  double GetKokoroBeginCost() const;
  double GetKokoroMaintainCost() const;
  double GetMiruCost() const;
  bool HasGod() const { return power_grade_.god; }

private:
  const PowerGrade& power_grade_;

};

}
