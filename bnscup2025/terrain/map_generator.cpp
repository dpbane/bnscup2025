#include "stdafx.h"
#include "map_generator.hpp"

namespace bnscup2025::terrain {

MapGenerator::Parameters MapGenerator::Generate(const Size& size, uint64 seed, int octaves, double persistence, double scale) {
  NodeGrid grid { size };
  PerlinNoise noise;
  noise.reseed(seed);

  // 窓関数っぽいやつ（0~1の実数に対し、端で0、それ以外で1に近い関数を返す）
  auto WindowFunction = [](double x) -> double {
    constexpr double a = 8.0;
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 0.0;
    return (1 - (Pow(x, a) + Pow(1 - x, a))) / (1 - Pow(2, 1 - a));
  };

  // Perlinノイズに基づく地形生成
  for (const auto& p : step(size)) {
    double nx = (double)p.x / size.x * scale;
    double ny = (double)p.y / size.y * scale;
    double density = noise.octave2D0_1(nx, ny, octaves, persistence);

    density += 0.05;
    density *= WindowFunction((double)p.x / (size.x - 1));
    density *= WindowFunction((double)p.y / (size.y - 1));
    density = Clamp(density, 0.0, 1.0);

    MaterialEnum material = MaterialEnum::Normal;
    if (p.x == 0 || p.x == size.x - 1 || p.y == 0 || p.y == size.y - 1)  material = MaterialEnum::Bounds;

    grid.Set(
      p,
      NodeInfo {
        .density = density,
        .material = material
      }
    );
  }
  Terrain terrain { grid };

  // プレイヤーの初期位置決定

  // 敵の初期位置決定

  return Parameters {
    .terrain = std::move(terrain),
    .player_position = size / 2,
    .enemy_position = size / 2
  };
}

}
