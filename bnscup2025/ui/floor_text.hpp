#pragma once

namespace bnscup2025::ui {

class FloorText {
public:
  FloorText(const String& text);

  void Update();
  void Render() const;

private:
  String text_;
  double timer_ { 0.0 };
};

}
