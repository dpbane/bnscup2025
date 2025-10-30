#pragma once

#include "i_scene.hpp"

namespace bnscup2025::scene {

class GameOver : public App::Scene {
public:
  GameOver(const InitData& init_data);

  void update() override;
  void draw() const override;

private:
  double timer_ = 0.0;

};

}
