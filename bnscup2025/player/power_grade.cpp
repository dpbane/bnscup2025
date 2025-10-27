#include "stdafx.h"
#include "power_grade.hpp"

namespace bnscup2025::player {

PowerGrade::PowerGrade() :
  grade_levels_ { 0, 0, 0, 0, 0, 0, 0 } {
}

uint8& PowerGrade::operator[](PowerGradeItem item) {
  return grade_levels_[static_cast<size_t>(item)];
}

const uint8& PowerGrade::operator[](PowerGradeItem item) const {
  return grade_levels_[static_cast<size_t>(item)];
}

}
