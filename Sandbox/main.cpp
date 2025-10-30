# include <Siv3D.hpp>

void Main() {
  Scene::SetBackground(ColorF { 0.6, 0.8, 0.7 });

  Vec2 pos { 400, 300 };

  while (System::Update()) {
    ClearPrint();

    // マウスホイールのスクロール量
    Print << Mouse::Wheel();

    // マウスの水平ホイールのスクロール量
    Print << Mouse::WheelH();

    pos.y -= (Mouse::Wheel() * 10);
    pos.x += (Mouse::WheelH() * 10);

    RectF { Arg::center = pos, 100 }.draw(ColorF { 0.2 });
  }
}
