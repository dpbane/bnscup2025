# include <Siv3D.hpp> // Siv3D v0.6.16

#include "scene/title.hpp"
#include "scene/game.hpp"
#include "scene/gameover.hpp"

#include "fps_addon.hpp"

#include "input/input.hpp"

using namespace bnscup2025;

void Main() {
  constexpr Size scene_size { 1920, 1080 };
  //constexpr Size scene_size { 2560, 1440 };
  constexpr Size window_size = scene_size;

  Scene::SetBackground(Color { 0x10, 0x10, 0x10 });
  //Scene::SetBackground(Palette::Black);

  Scene::SetResizeMode(ResizeMode::Keep);
  Scene::Resize(scene_size);
  Window::Resize(window_size.x, window_size.y);
  Window::SetPos(0, 0);

  Scene::SetMaxDeltaTime(1.0 / 30);

  // 一部の特殊キーを無効化する
  System::SetTerminationTriggers(UserAction::CloseButtonClicked);
  ScreenCapture::SetShortcutKeys({ KeyPrintScreen });
  LicenseManager::DisableDefaultTrigger();

  scene::App manager;
  manager.add<scene::Title>(scene::SceneEnum::Title);
  manager.add<scene::Game>(scene::SceneEnum::Game);
  manager.add<scene::GameOver>(scene::SceneEnum::GameOver);

  manager.init(scene::SceneEnum::Title, 0);

  //Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimitAddon>(60));

  FontAsset::Register(U"Title", 256, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap);
  FontAsset::Register(U"Text", 64, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap);

  System::Update();
  while (System::Update()) {
    ClearPrint();
    input::Input::GetInstance().Update();
    if (not manager.update()) {
      break;
    }
    Print << Profiler::FPS();
  }
}

