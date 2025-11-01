#pragma once

namespace bnscup2025::terrain {

class LifeGame {
public:
  LifeGame(const Size& size, double p = 0.50);

  bool GetCell(const Point& pos) const;

private:
  void Randomize(double p);
  void Update();
  bool& At(const Point& pos);

private:
  Size size_;
  Array<bool> cells_;

};

}
