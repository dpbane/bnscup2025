#include "stdafx.h"
#include "title.hpp"

#include "render/lightbloom.hpp"
#include "render/blend_mode.hpp"

#include "input/input.hpp"

namespace bnscup2025::scene {

Title::Title(const InitData& init_data) :
  IScene(init_data) {
  for (auto& noise : noises_) noise.reseed(RandomUint64());
}

void Title::update() {
  const auto input_data = input::Input::GetInstance().GetData();

  time_ += Scene::DeltaTime();

  if (input_data.confirm_trigger) {
    getData() = CommonData {
      .next_level = 0,
      .next_room = Room::Shop,
      .power_grade = player::PowerGrade{}
    };
    changeScene(SceneEnum::Game, 0);
  }
}

void Title::draw() const {
  const String texts[] = { U"神", U"出", U"鬼", U"没" };
  const ColorF colors[] = {
    ColorF { 0.11, 0.90, 0.50 },
    ColorF { 0.20, 0.80, 0.50 },
    ColorF { 0.90, 0.11, 0.50 },
    ColorF { 0.80, 0.20, 0.50 }
  };
  const double gryph_size = Scene::Height() / 1080.0 * 256.0;

  {
    const auto target = render::LightBloom::GetInstance().CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();

    for (int k = 0; k < 4; ++k) {
      FontAsset(U"Title")(texts[k]).drawAt(
        Scene::Center().movedBy(gryph_size * (-1.5 + k * 1.0), -Scene::Height() * 0.25),
        colors[k]
      );
    }
  }

  render::LightBloom::GetInstance().Apply(1.0, 0.0, 1.0);

}

}
