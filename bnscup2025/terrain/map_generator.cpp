#include "stdafx.h"
#include "map_generator.hpp"

namespace bnscup2025::terrain {

MapGenerator::Parameters MapGenerator::Generate(int level) {

  const auto node_grid = CreateNodeGrid(Size { 50, 50 }, RandomUint64(), 2, 0.5, 10.0);
  Terrain terrain = { node_grid, std::move(CreateSinhalitePositions(node_grid, 10)) };
  const auto pos_pair = CreatePlayerAndEnemyPosition(terrain);

  return Parameters {
    .terrain = std::move(terrain),
    .player_position = pos_pair.first,
    .enemy_position = pos_pair.second,
  };
}

double MapGenerator::WindowFunction(double x) {
  constexpr double a = 8.0;
  if (x < 0.0) return 0.0;
  if (x > 1.0) return 0.0;
  return (1 - (Pow(x, a) + Pow(1 - x, a))) / (1 - Pow(2, 1 - a));
}

NodeGrid MapGenerator::CreateNodeGrid(const Size& size, uint64 seed, int octaves, double persistence, double scale) {
  NodeGrid grid { size };
  PerlinNoise noise;
  noise.reseed(seed);

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

  return grid;
}

std::pair<Vec2, Vec2> MapGenerator::CreatePlayerAndEnemyPosition(const Terrain& terrain) {
  // アクセスマップを取得し、領域ごとに分割した配列を取得
  const auto& access_map = terrain.GetAccessMap();
  auto areas_array = access_map.CreateAreasArray();
  assert(not areas_array.empty());

  // 領域が大きい順にソートし、最も大きい領域を取得
  std::sort(
    areas_array.begin(),
    areas_array.end(),
    [](const Array<Point>& a, const Array<Point>& b) {
    return a.size() > b.size();
  }
  );
  const auto& biggest_area = areas_array[0];

  // プレイヤーの位置を確定：ランダムに選び、周囲が広い場所を探す
  Point player_point {};
  while (true) {
    player_point = biggest_area.choice();
    if (not biggest_area.contains(player_point.movedBy(-2, 0)) ||
      not biggest_area.contains(player_point.movedBy(2, 0)) ||
      not biggest_area.contains(player_point.movedBy(0, -2)) ||
      not biggest_area.contains(player_point.movedBy(0, 2))) {
      continue;
    }
    break;
  }

  // 敵の位置を確定：ランダムに選び、プレイヤーから十分に離れた場所を探す
  Point enemy_point {};
  double distance = 20.0;
  while (true) {
    enemy_point = biggest_area.choice();
    if (enemy_point.distanceFrom(player_point) < distance) {
      distance -= 0.1;
      continue;
    }
    break;
  }

  return { player_point, enemy_point };
}

Vec2 MapGenerator::CreateExitPosition(const NodeGrid& node_grid, const Vec2 player_pos) {
  Size size = node_grid.GetSize();
  return size / 2;
}

Array<Point> MapGenerator::CreateSinhalitePositions(const NodeGrid& node_grid, int count) {
  Size size = node_grid.GetSize();
  Array<Point> ret;

  for (int k = 0; k < count; ++k) {
    ret.emplace_back(Random(1, size.x - 1), Random(1, size.y - 1));
  }

  return ret;
}

Array<Point> MapGenerator::GetBiggestArea(const NodeGrid& node_grid) {
  return Array<Point>();
}

}
