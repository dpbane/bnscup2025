#pragma once

#include "camera/camera.hpp"
#include "player/player.hpp"

namespace bnscup2025::npc {

enum class SpeakerEnum {
  Orrange,
  Sky
};

class Speaker {
public:
  Speaker(const camera::Camera& camera, const player::Player& player, const Vec2& position, const Array<String>& texts, SpeakerEnum speaker_enum);

  void Update();
  void Render() const;

private:
  const camera::Camera& camera_;
  const player::Player& player_;
  SpeakerEnum speaker_enum_;

  const Vec2 position_;

  Array<String> texts_;
  size_t current_text_index_ { 0 };

};

}
