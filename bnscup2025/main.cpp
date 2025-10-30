# include <Siv3D.hpp> // Siv3D v0.6.16

#include "scene/title.hpp"
#include "scene/game.hpp"
#include "scene/gameover.hpp"

#include "fps_addon.hpp"

using namespace bnscup2025;

void Main() {
  constexpr Size scene_size { 1920, 1080 };
  constexpr Size window_size { 1920, 1080 };

  Scene::SetBackground(Color { 0x10, 0x10, 0x10 });
  //Scene::SetBackground(Palette::Black);


  Scene::Resize(scene_size);
  Window::Resize(window_size.x, window_size.y);
  Window::SetPos(0, 0);
  Scene::SetResizeMode(ResizeMode::Keep);

  scene::App manager;
  manager.add<scene::Title>(scene::SceneEnum::Title);
  manager.add<scene::Game>(scene::SceneEnum::Game);
  manager.add<scene::GameOver>(scene::SceneEnum::GameOver);

  manager.init(scene::SceneEnum::Title, 0);

  // Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimitAddon>(60));

  FontAsset::Register(U"Title", 256, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap);
  FontAsset::Register(U"Speaker", 64, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap);
  FontAsset::Register(U"Tutorial", 64, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap);

  System::Update();
  while (System::Update()) {
    ClearPrint();
    if (not manager.update()) {
      break;
    }
    Print << Profiler::FPS();
  }
}

