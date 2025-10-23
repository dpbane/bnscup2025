#pragma once

#include "terrain/terrain.hpp"
#include "terrain/visibility.hpp"
#include "camera/camera.hpp"

#include "player/player.hpp"

namespace bnscup2025::game {

class Game {
public:
  Game();

  void Update();
  void Render();

private:
  Optional<terrain::Terrain> terrain_;
  Optional<terrain::Visibility> visibility_;
  Optional<camera::Camera> camera_;

  Optional<player::Player> player_;

  MSRenderTexture visibility_mask_texture_;
  Array<Triangle> visibility_triangles_;

};

}
