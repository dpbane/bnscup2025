#include "stdafx.h"
#include "selector.hpp"

#include "input/input.hpp"

namespace bnscup2025::player {

Selector::Selector(const PowerGrade& power_grade) :
  power_grade_(power_grade) {
}

void Selector::Update() {
  const auto input_data = input::Input::GetInstance().GetData();


}

void Selector::Render() const {
  // 特に何もしない
}


}
