#include "stdafx.h"
#include "map_generator.hpp"

namespace bnscup2025::terrain {

NodeGrid MapGenerator::Generate(const Size& size, uint64 seed, int octaves, double persistence, double scale) {
  NodeGrid ret(size);
  PerlinNoise noise;
  noise.reseed(seed);

  for (const auto& p : step(size)) {
    double nx = (double)p.x / size.x * scale;
    double ny = (double)p.y / size.y * scale;
    double value = noise.octave2D0_1(nx, ny, octaves, persistence);

    if (p.x == 0 || p.x == size.x - 1 || p.y == 0 || p.y == size.y - 1) {
      value = 0.0;  // 境界は必ず地面にする
    }
    ret.Set(p, value);
  }


  return ret;
}

}
