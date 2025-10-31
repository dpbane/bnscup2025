#pragma once

#include "player/grade_value_converter.hpp"

namespace bnscup2025::player {

class EnergyUI {
public:
  EnergyUI(const PowerGrade& power_grade, const double& energy);

  void Update();
  void Render(Optional<double> burnout_rate, double radius) const;

private:
  const GradeValueConverter gvc_;
  const double& energy_;

  double alpha_timer_ { 1.0 };
  double alpha_ { 0.0 };

};

}
