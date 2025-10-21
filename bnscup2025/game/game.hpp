#pragma once

#include "terrain/terrain.hpp"

namespace bnscup2025::game {

class Game {
public:
  Game();

  void Update();
  void Render();

private:
  terrain::Terrain terrain_;

  SizeF scale_ { 50.0, 50.0 };
  Vec2 center_ { 0.0, 0.0 };

  MSRenderTexture visibility_mask_texture_;

};

}
