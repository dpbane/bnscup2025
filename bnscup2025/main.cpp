# include <Siv3D.hpp> // Siv3D v0.6.16

#include "scene/title.hpp"
#include "scene/game.hpp"
#include "scene/gameover.hpp"
#include "scene/ending.hpp"

#include "fps_addon.hpp"

#include "debug_var.hpp"
#include "input/input.hpp"

using namespace bnscup2025;

void Main() {
  //constexpr Size scene_size { 1920, 1080 };
  //constexpr Size scene_size { 2560, 1440 };
  //constexpr Size window_size = scene_size;

  Window::SetTitle(U"神出鬼没");

  Scene::SetBackground(Color { 0x10, 0x10, 0x10 });
  //Scene::SetBackground(Palette::Black);

  //Scene::SetResizeMode(ResizeMode::Keep);
  //Scene::Resize(scene_size);
  //Window::Resize(window_size.x, window_size.y);
  //Window::SetPos(0, 0);

  // 最低FPSは30（時間ベースのため低すぎると壁抜けなどが起こり得る）
  Scene::SetMaxDeltaTime(1.0 / 30);

  // 一部の特殊キーを無効化
  //System::SetTerminationTriggers(UserAction::CloseButtonClicked);
  ScreenCapture::SetShortcutKeys({ KeyPrintScreen });
  LicenseManager::DisableDefaultTrigger();

  scene::App manager;
  manager.add<scene::Title>(scene::SceneEnum::Title);
  manager.add<scene::Game>(scene::SceneEnum::Game);
  manager.add<scene::GameOver>(scene::SceneEnum::GameOver);
  manager.add<scene::Ending>(scene::SceneEnum::Ending);
  manager.init(scene::SceneEnum::Title, 0);

  //Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimitAddon>(60));

  FontAsset::Register(U"Title", 256, Resource(U"example/font/DotGothic16/DotGothic16-Regular.ttf"), FontStyle::Bitmap);
  FontAsset::Register(U"Text", 64, Resource(U"example/font/DotGothic16/DotGothic16-Regular.ttf"), FontStyle::Bitmap);

  System::Update();
  while (System::Update()) {
    ClearPrint();
    input::Input::GetInstance().Update();

    /*
    if (KeyF1.down()) DebugVar::GetInstance().disable_visibility_mask_ = not DebugVar::GetInstance().disable_visibility_mask_;
    if (KeyF2.down()) DebugVar::GetInstance().invincible_mode_ = true;
    if (KeyF3.down()) DebugVar::GetInstance().invincible_mode_ = false;
    if (KeyF4.down()) DebugVar::GetInstance().full_goshin_on_ = true;
    if (KeyF5.down()) DebugVar::GetInstance().visible_enemy_cost_map_ = not DebugVar::GetInstance().visible_enemy_cost_map_;
    if (KeyF6.down()) manager.changeScene(scene::SceneEnum::Game, 0);
    */

    if (not manager.update()) {
      break;
    }
    Print << Profiler::FPS();
  }
}

