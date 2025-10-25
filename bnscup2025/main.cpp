# include <Siv3D.hpp> // Siv3D v0.6.16

#include "game/game.hpp"

#include "fps_addon.hpp"

using namespace bnscup2025;

void Main() {
  constexpr Size window_size { 1920, 1080 };
  Window::Resize(window_size.x, window_size.y);

  Scene::SetBackground(Color { 0x10, 0x10, 0x10 });
  //Scene::SetBackground(Palette::Black);

  Window::SetPos(0, 0);
  //Scene::SetResizeMode(ResizeMode::Keep);
  //Scene::Resize(600, 720);

  game::Game game;

  //Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimitAddon>(60));

  System::Update();
  while (System::Update()) {
    ClearPrint();
    game.Update();
    game.Render();
    Print << Profiler::FPS();
  }
}

