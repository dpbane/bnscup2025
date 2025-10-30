#pragma once

namespace bnscup2025::ui {

class SinhaliteText {
public:
  SinhaliteText(const int& sinhalite_amount);

  void Update();
  void Render() const;

private:
  const int& sinhalite_amount_;
  double alpha_ { 0.0 };
};

}
