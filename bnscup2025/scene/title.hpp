#pragma once

#include "i_scene.hpp"

namespace bnscup2025::scene {

class Title : public App::Scene {
public:
  Title(const InitData& init_data);

  void update() override;
  void draw() const override;

private:
  double time_ = 0.0;
  std::array<PerlinNoise, 4> noises_;

};

}
