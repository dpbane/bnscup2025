#pragma once

namespace bnscup2025::ui {

class TextWindow {
  TextWindow();
  TextWindow(const TextWindow&) = delete;
  TextWindow& operator=(const TextWindow&) = delete;

public:
  static TextWindow& GetInstance() {
    static TextWindow instance;
    return instance;
  }

  void SetText(const String& text);
  void SetActive();

  void Reset();
  void Update();
  void Render() const;

  bool IsFadeFinished() const { return text_alpha_ >= 1.0; }

private:
  String current_text_;
  bool is_active_ { false };
  double text_window_alpha_ { 0.0 };
  double text_alpha_ { 0.0 };

};

}
