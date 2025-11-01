#include "stdafx.h"
#include "title.hpp"

#include "debug_var.hpp"

#include "render/lightbloom.hpp"
#include "render/blend_mode.hpp"

#include "input/input.hpp"
#include "screen/fade.hpp"

namespace bnscup2025::scene {

Title::Title(const InitData& init_data) :
  IScene(init_data) {
  screen::Fade::GetInstance().BeginFadeIn(kFadeDuration);
}

void Title::update() {
  const auto& input_data = input::Input::GetInstance().GetData();
  auto& fade = screen::Fade::GetInstance();

  if (fade.CompletedFadeIn() && input_data.confirm_trigger) {
    getData() = CommonData {
      .next_level = 0,
      .next_room = Room::Game,
      .power_grade = player::PowerGrade{},
      .sinhalite_amount = 0,
      .death_count = 0,
      .clear_count = 0
    };
    if (DebugVar::GetInstance().full_goshin_on_) {
      // デバッグ用
      player::PowerGrade pg;
      pg[player::PowerGradeItem::Nobiru] = 1;
      pg[player::PowerGradeItem::Kokoro] = 1;
      pg[player::PowerGradeItem::Susumu] = 1;
      pg[player::PowerGradeItem::Tsutsu] = 1;
      pg[player::PowerGradeItem::Miru] = 1;
      //pg[player::PowerGradeItem::God] = 1;
      pg[player::PowerGradeItem::ViewDistance] = 3;
      getData() = CommonData {
        .next_level = 1,
        .next_room = Room::Game,
        .power_grade = pg,
        .sinhalite_amount = 200,
        .death_count = 0,
        .clear_count = 0
      };
    }
    fade.BeginFadeOut(kFadeDuration);
  }

  if (fade.CompletedFadeOut()) {
    changeScene(SceneEnum::Game, 0);
  }

  fade.Update();
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
        gryph_size,
        Scene::CenterF().movedBy(gryph_size * (-1.5 + k * 1.0), -Scene::Height() * 0.25),
        colors[k].withA(0.7)
      );
    }
  }

  render::LightBloom::GetInstance().Apply(1.0, 0.0, 1.0);

  screen::Fade::GetInstance().Render();
}

}
