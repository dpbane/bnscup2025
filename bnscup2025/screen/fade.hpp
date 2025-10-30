#pragma once

namespace bnscup2025::screen {

class Fade {
  Fade();
  Fade(const Fade&) = delete;
  Fade& operator=(const Fade&) = delete;

public:
  static Fade& GetInstance();

  void Update();
  void Render() const;

  void BeginFadeIn(double fade_duration);
  void BeginFadeOut(double fade_duration);

  bool IsFadingIn() const { return is_fading_in_; }
  bool IsFadingOut() const { return is_fading_out_; }
  bool CompletedFadeIn() const;
  bool CompletedFadeOut() const;

private:
  double alpha_ { 1.0 };
  double fade_duration_ { 1.0 };
  bool is_fading_in_ { false };
  bool is_fading_out_ { false };
};

}
