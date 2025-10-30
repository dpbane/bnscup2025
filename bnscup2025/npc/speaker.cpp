#include "stdafx.h"
#include "speaker.hpp"

#include "input/input.hpp"
#include "render/chara_renderer.hpp"

#include "ui/text_window.hpp"

namespace bnscup2025::npc {

constexpr double kSpeakDistance = 6.0;

Speaker::Speaker(const camera::Camera& camera, const player::Player& player, const Vec2& position, const Array<String>& texts, SpeakerEnum speaker_enum) :
  camera_(camera),
  player_(player),
  speaker_enum_(speaker_enum),
  position_(position),
  texts_(texts) {
}

void Speaker::Update() {
  const auto input_data = input::Input::GetInstance().GetData();
  const Vec2 player_pos = player_.GetPosition();
  const double distance = (player_pos - position_).length();
  auto& text_window = ui::TextWindow::GetInstance();

  if (distance <= (kSpeakDistance)) {
    ui::TextWindow::GetInstance().SetText(texts_[current_text_index_]);
    text_window.SetActive();

    if (ui::TextWindow::GetInstance().IsFadeFinished() && current_text_index_ < texts_.size() - 1 && input_data.confirm_trigger) {
      ++current_text_index_;
    }
    ui::TextWindow::GetInstance().SetText(texts_[current_text_index_]);
  }
}

void Speaker::Render() const {

  const Vec2 direction = (player_.GetPosition() - position_).normalized();
  ColorF color_body {};
  ColorF color_edge {};
  switch (speaker_enum_) {
    case SpeakerEnum::Orrange:
      color_body = ColorF { 0.05, 0.03, 0.01 };
      color_edge = ColorF { 0.9, 0.4, 0.1 };
      break;
    case SpeakerEnum::Sky:
      color_body = ColorF { 0.01, 0.03, 0.05 };
      color_edge = ColorF { 0.1, 0.4, 0.9 };
      break;
  }

  render::CharaRenderer::Render(camera_, position_, direction, color_body, color_edge, 1.0, 1.0, 1.0);

}





}
