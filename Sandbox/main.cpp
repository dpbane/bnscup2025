# include <Siv3D.hpp> // Siv3D v0.6.16


void Main() {
	constexpr Size window_size{ 1280, 720 };
	Window::Resize(window_size.x, window_size.y);

	Scene::SetBackground(Color{ 0x10, 0x10, 0x10 });
	//Scene::SetBackground(Palette::Black);

	Window::SetPos(0, 0);
	//Scene::SetResizeMode(ResizeMode::Keep);
	//Scene::Resize(600, 720);


	System::Update();
	while (System::Update()) {
		ClearPrint();

		Vec2 center = Scene::CenterF();
		Vec2 mouse = Cursor::PosF();
		Line l1{ center, mouse };
		l1.draw(ColorF{ 1.0 });

		Vec2 dp = mouse - center;
		Print << dp.getAngle();

		Print << Profiler::FPS();
	}
}

