#include "stdafx.h"
#include "map_generator.hpp"

namespace bnscup2025::terrain {

NodeGrid MapGenerator::Generate(const Size& size, int octaves, double persistence, double scale) {
  NodeGrid ret(size);
  PerlinNoise noise;

  for (int y = 0; y < size.y; ++y) {
    for (int x = 0; x < size.x; ++x) {
      double nx = static_cast<double>(x) / static_cast<double>(size.x) * scale;
      double ny = static_cast<double>(y) / static_cast<double>(size.y) * scale;
      double value = noise.octave2D0_1(nx, ny, octaves, persistence);

      if (x == 0 || x == size.x - 1 || y == 0 || y == size.y - 1) {
        value = 0.0;  // 境界は必ず地面にする
      }
      ret.Set(Point { x, y }, value);
    }
  }
  return ret;
}

}
