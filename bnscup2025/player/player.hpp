#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"

namespace bnscup2025::player {

class Player {
public:
  Player(const camera::Camera& camera, terrain::Terrain& terrain, Effect& effect, Vec2 pos, bool is_game);

  void Update();
  void Render() const;


  const Vec2& GetPosition() const { return position_; }
  const Vec2 GetShiftedPosition() const { return position_ + shift_amount_; }
  const Vec2& GetDirectionFace() const { return direction_face_; }

  const Optional<Vec2>& GetSoundPosition() const { return sound_position_; }

private:
  void ProcessMove();
  void ProcessShift();
  void ProcessDigging();

private:
  const camera::Camera& camera_;
  terrain::Terrain& terrain_;
  Effect& effect_;
  const bool is_game_;

  Vec2 position_;
  Vec2 shift_amount_{ 0.0, 0.0 };
  Vec2 direction_face_{ 0.0, -1.0 };

  double dig_timer_{ 0.0 };
  Optional<Vec2> digging_position_;

  Optional<Vec2> sound_position_;

};

}
