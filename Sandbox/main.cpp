# include <Siv3D.hpp>

void Main() {
  Window::Resize(1280, 720);
  Scene::SetBackground(ColorF { 0.6, 0.8, 0.7 });

  Size size { 4, 4 };
  for (const auto& point : step(size)) {
    Print << (point);
  }

  while (System::Update()) {
  }
}
