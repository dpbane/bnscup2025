#include "stdafx.h"
#include "lifegame.hpp"

namespace bnscup2025::terrain {

LifeGame::LifeGame(const Size& size, double p) :
  size_(size) {
  cells_.resize(size_t(size.x * size.y), false);
  Randomize(p);
  for (int k = 0; k < 3; ++k) {
    Update();
  }
}

void LifeGame::Randomize(double pp) {
  for (const auto& p : step(size_)) {
    At(p) = RandomBool(pp);
  }
}

void LifeGame::Update() {
  // 上下左右ループあり
  auto Sum = [this](const Point& pos) {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
      for (int dx = -1; dx <= 1; ++dx) {
        if (dx == 0 && dy == 0) continue;
        Point neighbor_pos {
          (pos.x + dx + size_.x) % size_.x,
          (pos.y + dy + size_.y) % size_.y
        };
        if (GetCell(neighbor_pos)) {
          ++count;
        }
      }
    }
    return count;
  };

  for (const auto& p : step(size_)) {
    const int sum = Sum(p);
    if (sum >= 5) {
      At(p) = true;
    }
    else if (sum <= 3) {
      At(p) = false;
    }
  }
}

bool LifeGame::GetCell(const Point& pos) const {
  if (pos.x < 0 || pos.x >= size_.x || pos.y < 0 || pos.y >= size_.y) return false;
  return cells_[size_t(pos.y * size_.x + pos.x)];
}

bool& LifeGame::At(const Point& pos) {
  return cells_[size_t(pos.y * size_.x + pos.x)];
}


}
