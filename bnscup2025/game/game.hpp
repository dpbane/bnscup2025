#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"

namespace bnscup2025::game {

class Game {
public:
  Game();

  void Update();
  void Render();

private:
  terrain::Terrain terrain_;
  camera::Camera camera_;

  MSRenderTexture visibility_mask_texture_;

};

}
