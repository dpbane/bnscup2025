#pragma once

namespace bnscup2025::ui {

class SinhaliteText {
public:
  SinhaliteText(const int& sinhalite_amount, bool visible);

  void Update();
  void Render() const;

private:
  const int& sinhalite_amount_;
  int last_amount_;
  int plus_amount_ { 0 };

  bool visible_;
  double alpha_ { 0.0 };

  double plus_timer_ { 0.0 };
};

}
