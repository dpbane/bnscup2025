#include "stdafx.h"
#include "map_generator.hpp"

#include "generator/level_designer.hpp"
#include "generator/lifegame.hpp"

#include "marching_squares.hpp"
#include "access_map.hpp"

namespace bnscup2025::terrain {

MapGenerator::Parameters MapGenerator::Generate(int level, bool is_game) {

  if (is_game) {
    if (level == 0) {
      NodeGrid node_grid = CreateTutorialNodeGrid();
      Terrain terrain { node_grid, level };
      return Parameters {
      .terrain = std::move(terrain),
      .player_position = Vec2{9.5, 65.0},
      .enemy_position = Vec2{-10.0, -10.0},
      .exit_position = Vec2{9.5, 6.0},
      .speaker_position = {},
      .shop_position = {}
      };
    }
    else {
      const auto params = LevelDesigner::MakeParameters(level);

      while (true) {
        NodeGrid node_grid = CreateNodeGrid(params.size);
        Terrain terrain = { node_grid, level };
        const auto& access_map = terrain.GetAccessMap();
        const auto areas = access_map.CreateAreasArray();
        if (areas.size() < 2) continue;  // 連続領域が1つ以下の場合はやり直し

        const auto& largest_area = areas[0];
        double area_ratio = static_cast<double>(largest_area.size()) / static_cast<double>(params.size.x * params.size.y);
        if (area_ratio < params.biggest_area_ratio_min) continue;
        if (area_ratio > params.biggest_area_ratio_max) continue;

        const auto positions = CreatePlayerAndEnemyPosition(terrain, params.enemy_distance);
        if (not positions) continue;

        terrain.SetSinhalitePositions(CreateSinhalitePositions(terrain, params.sinhalite_count));
        return Parameters {
        .terrain = std::move(terrain),
        .player_position = positions->player_position,
        .enemy_position = positions->enemy_position,
        .exit_position = positions->exit_position,
        .speaker_position = {},
        .shop_position = {}
        };
      }
    }
  }
  else {
    NodeGrid node_grid = CreateShopNodeGrid(level);
    Terrain terrain { node_grid, level };
    return Parameters {
    .terrain = std::move(terrain),
    .player_position = Vec2{14.5, 20},
    .enemy_position = Vec2{},
    .exit_position = level < 12 ? Vec2{14.5, 3} : Vec2{-100000, -100000},
    .speaker_position = level == 0 ? Vec2{ 16, 11.5 } : Vec2 { 21, 11.5 },
    .shop_position = Vec2 { 5, 11.5 }
    };
  }

}

double MapGenerator::WindowFunction(double x) {
  constexpr double a = 8.0;
  if (x < 0.0) return 0.0;
  if (x > 1.0) return 0.0;
  return (1 - (Pow(x, a) + Pow(1 - x, a))) / (1 - Pow(2, 1 - a));
}

NodeGrid MapGenerator::CreateNodeGrid(const Size& size) {
  NodeGrid ret { size };
  GridPoints<double> grid { size };

  // セルオートマトンによる地形生成
  LifeGame life_game { size * 2, 0.50 };
  for (const auto& p : step(grid.GetSize())) {
    int sum = 0;
    if (life_game.GetCell((p * 2).movedBy(0, 0))) ++sum;
    if (life_game.GetCell((p * 2).movedBy(1, 0))) ++sum;
    if (life_game.GetCell((p * 2).movedBy(0, 1))) ++sum;
    if (life_game.GetCell((p * 2).movedBy(1, 1))) ++sum;
    double density = 0.40 + (sum - 0.5) * 0.2;
    grid.Set(p, density);
  }

  // 平滑化
  for (int k = 0; k < 1; ++k) {
    for (const auto& p : step(grid.GetSize())) {
      double sum = 0.0;
      int count = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          Point neighbor_pos { p.x + dx, p.y + dy };
          if (not grid.IsBound(neighbor_pos)) continue;
          sum += grid.Get(neighbor_pos);
          ++count;
        }
      }
      double average = sum / static_cast<double>(count);
      grid.Set(p, average);
    }
  }

  // 窓関数を適用
  for (const auto& p : step(grid.GetSize())) {
    const double nx = static_cast<double>(p.x) / static_cast<double>(size.x - 1);
    const double ny = static_cast<double>(p.y) / static_cast<double>(size.y - 1);
    const double wx = WindowFunction(nx);
    const double wy = WindowFunction(ny);
    const double w = wx * wy;
    const double density = grid.Get(p) * w;
    grid.Set(p, density);
  }


  for (const auto& p : step(grid.GetSize())) {
    MaterialEnum material = MaterialEnum::Normal;
    if (p.x == 0 || p.x == grid.GetSize().x - 1 || p.y == 0 || p.y == grid.GetSize().y - 1) {
      material = MaterialEnum::Bounds;
    }

    ret.Set(
      p,
      NodeInfo {
        .density = grid.Get(p),
        .material = material
      }
    );
  }


  return ret;
}

NodeGrid MapGenerator::CreateShopNodeGrid(int level) {
  NodeGrid grid { Size {30, 30} };

  for (const auto& p : step(grid.GetSize())) {
    double density = 0;
    if (level < 12) if (p.x >= 12 && p.x <= 17 && p.y >= 1 && p.y < 8) density = 1.0;
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
  NodeGrid grid { Size {20, 70} };

  for (const auto& p : step(grid.GetSize())) {
    double density = 1.0;
    MaterialEnum material = MaterialEnum::Normal;

    // Circle
    Circle circles_[] = {
      Circle{Vec2{4.0, 55.0}, 8.0},
      Circle{Vec2{13.0, 40.0}, 12.0},
    };
    for (const auto& c : circles_) {
      const double dist_n = Point { p }.distanceFrom(c.center) / c.r;
      if (dist_n < 1.0) density = dist_n;
    }

    // Box
    if (p.x >= 1 && p.x <= 18 && p.y >= 14 && p.y <= 22) {
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

Optional<MapGenerator::Positions> MapGenerator::CreatePlayerAndEnemyPosition(const Terrain& terrain, double enemy_distance) {
  // アクセスマップを取得し、領域ごとに分割した配列を取得
  const auto& access_map = terrain.GetAccessMap();
  auto areas_array = access_map.CreateAreasArray();
  assert(not areas_array.size() < 2);

  // 最も大きい領域と2番目に大きい領域をシャッフル
  auto& biggest_area = areas_array[0];
  auto& second_area = areas_array[1];
  biggest_area.shuffle();
  second_area.shuffle();
  size_t biggest_index { 0 };
  size_t second_index { 0 };

  // プレイヤーの位置を確定：ランダムに選び、周囲が広い場所を探す
  Point player_point {};
  while (true) {
    if (biggest_index >= biggest_area.size()) return none;
    player_point = biggest_area[biggest_index++];
    Vec2 test_pos = terrain.Pushback(Circle { player_point, kCharacterRadius * 2 });
    if (test_pos == player_point) break;
  }

  // 敵の位置を確定：ランダムに選び、プレイヤーから十分に離れた場所を探す
  Point enemy_point {};
  while (true) {
    if (biggest_index >= biggest_area.size()) return none;
    enemy_point = biggest_area[biggest_index++];
    if (enemy_point.distanceFrom(player_point) < enemy_distance) continue;
    break;
  }

  // 出口の位置を確定：2番目に大きい領域からランダム
  Point exit_point {};
  while (true) {
    if (second_index >= second_area.size()) return none;
    exit_point = second_area[second_index++];
    Vec2 test_pos = terrain.Pushback(Circle { exit_point, kCharacterRadius * 2 });
    if (test_pos == exit_point) break;
  }

  return Positions {
    .player_position = player_point,
    .enemy_position = enemy_point,
    .exit_position = exit_point
  };
}

Array<Point> MapGenerator::CreateSinhalitePositions(const Terrain& terrain, int count) {
  Array<Point> ret;
  ret.reserve(count);

  // 最大領域と2番目の領域を取得
  const auto& access_map = terrain.GetAccessMap();
  auto areas_array = access_map.CreateAreasArray();
  assert(areas_array.size() >= 2);
  auto& biggest_area = areas_array[0];
  auto& second_area = areas_array[1];

  // 各領域のうち、エッジにあたる部分を抽出
  Array<Point> biggest_edge_points;
  Array<Point> second_edge_points;
  auto MakeEdge = [&](const Array<Point>& area, Array<Point>& edge_points) {
    constexpr Point directions[] = {
      Point{ -1, 0 }, Point{ 1, 0 }, Point{ 0, -1 }, Point{ 0, 1 },
      Point{ -1, -1 }, Point{ -1, 1 }, Point{ 1, -1 }, Point{ 1, 1 }
    };
    for (const auto& p : area) {
      for (const auto& d : directions) {
        Point neighbor_pos = p + d;
        if (terrain.GetCase(neighbor_pos) != MarchingSquares::Case::Full) {
          edge_points.push_back(p);
          break;
        }
      }
    }
  };
  MakeEdge(biggest_area, biggest_edge_points);
  MakeEdge(second_area, second_edge_points);

  // 半径rangeの範囲の格子点を事前計算
  const double range = 7.0;
  const int range_i = static_cast<int>(Floor(range));
  Array<Point> offset_points;
  for (int dy = -range_i; dy <= range_i; ++dy) {
    for (int dx = -range_i; dx <= range_i; ++dx) {
      if (dx * dx + dy * dy <= range * range) {
        offset_points.push_back(Point { dx, dy });
      }
    }
  }

  // ランダムなエッジから適当な距離にシンハライトを配置
  second_edge_points.shuffle();
  size_t second_index = 0;
  while (ret.size() < 8) {
    const Point edge_point = second_edge_points[second_index++];
    for (const auto& offset : offset_points.shuffled()) {
      const Point sinhalite_point = edge_point + offset;
      if (terrain.GetCase(sinhalite_point) == MarchingSquares::Case::None) {
        ret.push_back(sinhalite_point);
        break;
      }
    }
  }
  biggest_edge_points.shuffle();
  size_t biggest_index = 0;
  while (ret.size() < count) {
    const Point edge_point = biggest_edge_points[biggest_index++];
    for (const auto& offset : offset_points.shuffled()) {
      const Point sinhalite_point = edge_point + offset;
      if (terrain.GetCase(sinhalite_point) == MarchingSquares::Case::None) {
        ret.push_back(sinhalite_point);
        break;
      }
    }
  }

  return ret;
}

}
