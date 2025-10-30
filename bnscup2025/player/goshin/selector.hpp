#pragma once

#include "player/power_grade.hpp"

namespace bnscup2025::player {

class Selector {
public:
  Selector(const PowerGrade& power_grade);

  void Update();
  void Render() const;

private:
  const PowerGrade& power_grade_;

  double disp_offset_ { 0.0 };

};

}
