#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"
#include "player/player.hpp"

namespace bnscup2025::enemy {

class Enemy {
public:
  Enemy(const camera::Camera& camera,
    const terrain::Terrain& terrain,
    const player::Player& player,
    const Vec2& pos);


  void Update();
  void Render() const;

private:
  void ThreadMain();


private:
  const camera::Camera& camera_;
  const terrain::Terrain& terrain_;
  const player::Player& player_;

  Vec2 position_;
  Vec2 direction_face_ { 0.0, -1.0 };

  std::thread thread_;

};

}
