# include <Siv3D.hpp>

void Main() {
  Window::Resize(1280, 720);
  Scene::SetBackground(ColorF { 0.6, 0.8, 0.7 });

  Image image1 { 512, 512, Palette::White };
  Image image2 { 512, 512, Palette::White };
  DynamicTexture texture1 { image1 };
  DynamicTexture texture2 { image2 };

  PerlinNoise noise;
  size_t oct = 5;
  double persistence = 0.5;
  const Array<String> options = Range(1, 6).map(Format);

  while (System::Update()) {
    SimpleGUI::Headline(U"octaves", Vec2 { 40, 540 });
    SimpleGUI::HorizontalRadioButtons(oct, options, Vec2 { 40, 580 });

    SimpleGUI::Headline(U"persistence", Vec2 { 400, 540 });
    SimpleGUI::Slider(U"{:.2f}"_fmt(persistence), persistence, Vec2 { 400, 580 }, 60, 120);

    if (SimpleGUI::Button(U"Generate", Vec2 { 620, 580 })) {
      noise.reseed(RandomUint64());
      const int32 octaves = static_cast<int32>(oct + 1);

      for (auto p : step(image1.size())) {
        image1[p] = ColorF { noise.normalizedOctave2D0_1((p / 128.0), octaves, persistence) };
      }
      for (auto p : step(image2.size())) {
        image2[p] = ColorF { noise.octave2D0_1((p / 128.0), octaves, persistence) };
      }
      texture1.fill(image1);
      texture2.fill(image2);
    }

    texture1.draw();
    texture2.draw(512, 0);
  }
}
