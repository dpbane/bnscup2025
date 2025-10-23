#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"

namespace bnscup2025::player {

class Player {
public:
  Player(const camera::Camera& camera, terrain::Terrain& terrain, Vec2 pos);

  void Update();
  void Render() const;


  const Vec2& GetPosition() const { return position_; }
  const Vec2& GetDirectionFace() const { return direction_face_; }

private:
  void ProcessDigging();

private:
  const camera::Camera& camera_;
  terrain::Terrain& terrain_;

  Vec2 position_;
  Vec2 direction_face_ { 0.0, -1.0 };

  double dig_timer_ { 0.0 };
  Optional<Vec2> digging_position_;

};

}
