#pragma once

#include "i_scene.hpp"

namespace bnscup2025::scene {

class Ending : public App::Scene {
public:
  Ending(const InitData& init_data);

  void update() override;
  void draw() const override;

private:
  size_t current_line_index_ { 0 };
  Array<String> str_lines_;

  bool finished_ { false };
  double timer_ { 0.0 };
};

}
