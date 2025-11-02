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

  switch (getData().screen_size) {
    case ScreenSizeEnum::_540p:
      if (Scene::Height() != 540) {
        Scene::Resize(960, 540);
        Window::Resize(960, 540);
      }
      break;

    case ScreenSizeEnum::_720p:
      if (Scene::Height() != 720) {
        Scene::Resize(1280, 720);
        Window::Resize(1280, 720);
      }
      break;

    case ScreenSizeEnum::_1080p:
      if (Scene::Height() != 1080) {
        Scene::Resize(1920, 1080);
        Window::Resize(1920, 1080);
      }
      break;

    case ScreenSizeEnum::_1440p:
      if (Scene::Height() != 1440) {
        Scene::Resize(2560, 1440);
        Window::Resize(2560, 1440);
      }
      break;
  }
  render::LightBloom::GetInstance().Reset();

}

void Title::update() {
  const auto& input_data = input::Input::GetInstance().GetData();
  auto& fade = screen::Fade::GetInstance();

  if (fade.CompletedFadeIn()) {
    if (input_data.confirm_trigger) {
      getData() = CommonData {
        .screen_size = getData().screen_size,
        .next_level = 0,
        .next_room = Room::Game,
        .power_grade = player::PowerGrade{},
        .sinhalite_amount = 0,
        .death_count = 0,
        .clear_count = getData().clear_count
      };
      if (DebugVar::GetInstance().full_goshin_on_) {
        // デバッグ用
        player::PowerGrade pg;
        pg[player::PowerGradeItem::Nobiru] = 1;
        pg[player::PowerGradeItem::Kokoro] = 1;
        pg[player::PowerGradeItem::Susumu] = 1;
        pg[player::PowerGradeItem::Tsutsu] = 1;
        //pg[player::PowerGradeItem::Miru] = 1;
        //pg[player::PowerGradeItem::God] = 1;
        pg[player::PowerGradeItem::MaxEnergy] = 5;
        pg[player::PowerGradeItem::ViewDistance] = 0;
        getData() = CommonData {
          .next_level = 3,
          .next_room = Room::Game,
          .power_grade = pg,
          .sinhalite_amount = 0,
          .death_count = 0,
          .clear_count = 0
        };
      }
      fade.BeginFadeOut(kFadeDuration);
    }
    else if (input_data.action_change_upright) {
      switch (getData().screen_size) {
        case ScreenSizeEnum::_540p:
          getData().screen_size = ScreenSizeEnum::_720p;
          Scene::Resize(1280, 720);
          Window::Resize(1280, 720);
          changeScene(SceneEnum::Title, 0);
          break;
        case ScreenSizeEnum::_720p:
          getData().screen_size = ScreenSizeEnum::_1080p;
          Scene::Resize(1920, 1080);
          Window::Resize(1920, 1080);
          changeScene(SceneEnum::Title, 0);
          break;
        case ScreenSizeEnum::_1080p:
          getData().screen_size = ScreenSizeEnum::_1440p;
          Scene::Resize(2560, 1440);
          Window::Resize(2560, 1440);
          changeScene(SceneEnum::Title, 0);
          break;
      }
    }
    else if (input_data.action_change_downleft) {
      switch (getData().screen_size) {
        case ScreenSizeEnum::_1440p:
          getData().screen_size = ScreenSizeEnum::_1080p;
          Scene::Resize(1920, 1080);
          Window::Resize(1920, 1080);
          changeScene(SceneEnum::Title, 0);
          break;
        case ScreenSizeEnum::_1080p:
          getData().screen_size = ScreenSizeEnum::_720p;
          Scene::Resize(1280, 720);
          Window::Resize(1280, 720);
          changeScene(SceneEnum::Title, 0);
          break;
        case ScreenSizeEnum::_720p:
          getData().screen_size = ScreenSizeEnum::_540p;
          Scene::Resize(960, 540);
          Window::Resize(960, 540);
          changeScene(SceneEnum::Title, 0);
          break;
      }
    }
  }

  timer_ += Scene::DeltaTime();

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


  const double alpha = 0.75 + 0.25 * Sin(timer_ * 5.0);
  const double sub_text_size = Scene::Height() / 1080.0 * 48.0;
  FontAsset(U"Text")(U"左右クリック / LB / RB でスタート").drawAt(
    sub_text_size,
    Scene::CenterF().movedBy(0.0, Scene::Height() * 0.25),
    ColorF { 1.0, 1.0, 1.0, alpha }
  );
  FontAsset(U"Text")(U"マウスホイール / LT / RT で解像度変更").drawAt(
    sub_text_size,
    Scene::CenterF().movedBy(0.0, Scene::Height() * 0.35),
    ColorF { 1.0, 1.0, 1.0, alpha }
  );

  screen::Fade::GetInstance().Render();
}

}
