#include "stdafx.h"
#include "map_generator.hpp"

namespace bnscup2025::terrain {

MapGenerator::Parameters MapGenerator::Generate(int level, bool is_game) {

  if (is_game) {
    if (level == 0) {
      NodeGrid node_grid = CreateTutorialNodeGrid();
      Terrain terrain = { node_grid, std::move(CreateSinhalitePositions(node_grid, 0)) };
      return Parameters {
      .terrain = std::move(terrain),
      .player_position = Vec2{9.5, 55.0},
      .enemy_position = Vec2{-10.0, -10.0},
      .exit_position = Vec2{9.5, 4.0},
      .speaker_position = {},
      .shop_position = {}
      };
    }
    else {
      NodeGrid node_grid = CreateNodeGrid(Size { 100, 100 }, RandomUint64(), 2, 0.8, 0.05);
      Terrain terrain = { node_grid, std::move(CreateSinhalitePositions(node_grid, 10)) };
      const auto positions = CreatePlayerAndEnemyPosition(terrain);
      return Parameters {
      .terrain = std::move(terrain),
      .player_position = positions.player_position,
      .enemy_position = positions.enemy_position,
      .exit_position = positions.exit_position,
      .speaker_position = {},
      .shop_position = {}
      };
    }
  }
  else {
    NodeGrid node_grid = CreateShopNodeGrid(level);
    Terrain terrain = { node_grid, std::move(Array<Point>{}) };
    return Parameters {
    .terrain = std::move(terrain),
    .player_position = Vec2{14.5, 20},
    .enemy_position = Vec2{},
    .exit_position = Vec2{14.5, 3},
    .speaker_position = Vec2 { 24, 11.5 },
    .shop_position = {}
    };
  }

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
    double nx = (double)p.x * scale;
    double ny = (double)p.y * scale;
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

NodeGrid MapGenerator::CreateShopNodeGrid(int level) {
  NodeGrid grid { Size {30, 30} };

  for (const auto& p : step(grid.GetSize())) {
    double density = 0;
    if (p.x >= 12 && p.x <= 17 && p.y >= 1 && p.y < 8) density = 1.0;
    if (p.x >= 1 && p.x < 29 && p.y >= 8 && p.y < 16) density = 1.0;
    if (p.x >= 9 && p.x <= 20 && p.y >= 16 && p.y <= grid.GetSize().y - 2) density = 1.0;

    grid.Set(
      p,
      NodeInfo {
        .density = density,
        .material = MaterialEnum::Normal
      }
    );
  }

  return grid;
}

NodeGrid MapGenerator::CreateTutorialNodeGrid() {
  NodeGrid grid { Size {20, 60} };

  for (const auto& p : step(grid.GetSize())) {
    double density = 1.0;
    MaterialEnum material = MaterialEnum::Normal;

    // Circle
    Circle circles_[] = {
      Circle{Vec2{4.0, 45.0}, 8.0},
      Circle{Vec2{13.0, 30.0}, 12.0},
    };
    for (const auto& c : circles_) {
      const double dist_n = Point { p }.distanceFrom(c.center) / c.r;
      if (dist_n < 1.0) density = dist_n;
    }

    // Box
    if (p.x >= 1 && p.x <= 18 && p.y >= 10 && p.y <= 12) {
      density = 0.4;
    }

    // Bounds
    if (p.x == 0 || p.x == grid.GetSize().x - 1 || p.y == 0 || p.y == grid.GetSize().y - 1) {
      density = 0.0;
      material = MaterialEnum::Bounds;
    }


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

MapGenerator::Positions MapGenerator::CreatePlayerAndEnemyPosition(const Terrain& terrain) {
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

  // 出口の位置を確定：2番目に大きい領域からランダム、または敵の位置と同一
  Point exit_point {};
  if (areas_array.size() >= 2) {
    const auto& exit_area = areas_array[1];
    bool found { false };
    for (const auto& pos : areas_array[1]) {
      if (not exit_area.contains(player_point.movedBy(-2, 0)) ||
        not exit_area.contains(player_point.movedBy(2, 0)) ||
        not exit_area.contains(player_point.movedBy(0, -2)) ||
        not exit_area.contains(player_point.movedBy(0, 2))) {
        continue;
      }
      exit_point = pos;
      found = true;
      break;
    }
    if (not found) exit_point = enemy_point;
  }
  else {
    exit_point = enemy_point;
  }

  return Positions {
    .player_position = player_point,
    .enemy_position = enemy_point,
    .exit_position = exit_point
  };
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
