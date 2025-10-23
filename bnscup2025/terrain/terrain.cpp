#include "stdafx.h"
#include "terrain.hpp"

#include <ranges>

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

namespace bnscup2025::terrain {

constexpr int kBlurRate = 4;

Terrain::Terrain(NodeGrid node_grid) :
  node_grid_(node_grid),
  marching_squares_(node_grid_, 0.5) {
}

void Terrain::Update() {
  marching_squares_.Update(node_grid_);
}

void Terrain::Render(const camera::Camera& cam) const {

  // 描画範囲セルの取得
  Array<Point> visible_cells = CreateVisibleCells(cam);

  // 各種描画
  RenderGround(visible_cells, cam);
  RenderGroundEdges(visible_cells, cam);
  //RenderWalls(visible_cells, wall_height);
  //RenderWallTopEdges(visible_cells, wall_height);

}

Size Terrain::GetNodeGridSize() const {
  return node_grid_.GetSize();
}

Size Terrain::GetCellGridSize() const {
  return marching_squares_.GetPolygons().GetSize();
}

MarchingSquares::Case Terrain::GetCase(const Point& pos) const {
  return marching_squares_.GetCaseGrid().Get(pos);
}

Array<Line> Terrain::CreateVisibleWallLines(const camera::Camera& cam) const {
  Array<Point> visible_cells = CreateVisibleCells(cam);
  const auto& lines = marching_squares_.GetEdgeLines();

  Array<Line> ret;
  for (const auto& pos : visible_cells) {
    const auto edge_lines = lines.Get(pos);
    for (const auto& vec_array : edge_lines) {
      for (size_t i = 0; i < vec_array.size() - 1; ++i) {
        ret.emplace_back(vec_array[i], vec_array[i + 1]);
      }
    }
  }
  return ret;
}

Vec2 Terrain::PushbackService(const Circle& circle) const {
  const auto rect = circle.boundingRect();
  const int x_min = static_cast<int>(std::floor(rect.leftX()));
  const int x_max = static_cast<int>(std::ceil(rect.rightX()));
  const int y_min = static_cast<int>(std::floor(rect.topY()));
  const int y_max = static_cast<int>(std::ceil(rect.bottomY()));

  Circle current_circle = circle;

  for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
      const Point cell_pos { x, y };
      const auto lines = marching_squares_.GetEdgeLines().Get(cell_pos);

      for (const auto& line_array : lines) {
        for (size_t i = 0; i < line_array.size() - 1; ++i) {
          const Line line { line_array[i], line_array[i + 1] };

          // Step 1: 円と線分が交差するかを確認し、領域の内外を判定する。
          // 線分は領域を反時計回りに囲むように配置されているので、外積で判定できる。
          if (not current_circle.intersects(line)) continue;
          const Vec2 line_dir = line.end - line.begin;
          const Vec2 to_circle_center = current_circle.center - line.begin;
          if (line_dir.cross(to_circle_center) < 0.0) continue;

          // Step 2: 円の中心を線分に投影したときの媒介変数tdを求める。
          // https://zenn.dev/boiledorange73/articles/0037-js-distance-pt-seg
          const auto& p1 = line.begin;
          const auto& p2 = line.end;
          const auto& p0 = current_circle.center;
          const double td = -(p2 - p1).dot(p1 - p0);

          // Step 3: 円の中心に最も近い線分上の点を求める。
          Vec2 nearest_point {};
          if (td <= 0.0) {
            nearest_point = p1;
          }
          else if (td >= (p2 - p1).lengthSq()) {
            nearest_point = p2;
          }
          else {
            const double t = td / (p2 - p1).lengthSq();
            nearest_point = p1 + (p2 - p1) * t;
          }

          // Step 4: 押し戻すためのベクトルを求める。
          const Vec2 pushback_vector = current_circle.center - nearest_point;
          const double pushback_distance = current_circle.r - pushback_vector.length();

          // Step 5: 押し戻した後の位置を返す。
          current_circle.center += pushback_vector.normalized() * pushback_distance;
        }
      }
    }
  }

  return current_circle.center;
}

Optional<Vec2> Terrain::CalcLineCollisionPoint(const Vec2& from, const Vec2& direction, double max_distance) const {
  const Line ray { from, from + direction * max_distance };

  // 次にグリッド線と交差するtを求める関数
  auto GetNextT = [&](double current_t) -> double {

    // 一方向のみについて考える関数
    auto GetNextTSub = [&](double current_pos, double velocity) -> Optional<double> {
      if (velocity == 0.0) return none;

      const double frac = current_pos - std::floor(current_pos);  // 小数部分
      if (velocity > 0.0) {
        const double dist_to_next_grid = 1.0 - frac;
        return current_t + dist_to_next_grid / velocity;
      }
      else {
        constexpr double epsilon = 1e-10;
        const double dist_to_prev_grid = frac + epsilon;
        return current_t + dist_to_prev_grid / -velocity;
      }
    };

    const Vec2 current_pos = from + direction * max_distance * current_t;
    const Optional<double> next_t_x = GetNextTSub(current_pos.x, direction.x * max_distance);
    const Optional<double> next_t_y = GetNextTSub(current_pos.y, direction.y * max_distance);

    if (not next_t_x || (next_t_y && next_t_y.value() < next_t_x.value())) {
      return *next_t_y;
    }
    else {
      return *next_t_x;
    }
  };

  double t = 0.0;  // 媒介変数
  while (t <= 1.0) {
    const Point cell_pos {
      static_cast<int>(std::floor((from.x + direction.x * max_distance * t))),
      static_cast<int>(std::floor((from.y + direction.y * max_distance * t)))
    };
    const auto lines = marching_squares_.GetEdgeLines().Get(cell_pos);
    for (const auto& line_array : lines) {
      for (size_t i = 0; i < line_array.size() - 1; ++i) {
        const Line line { line_array[i], line_array[i + 1] };
        const Optional<Vec2> intersection = ray.intersectsAt(line);
        if (intersection) {
          return intersection;
        }
      }
    }

    t = GetNextT(t);
  };

  return none;
}

void Terrain::DigAt(const Vec2& center, double radius, double center_might, double end_might) {
  // Step 1: 掘削範囲となるノードの候補を取得する。
  // 簡単のために、円に概説する正方形でノードを列挙。
  const int x_min = static_cast<int>(std::ceil(center.x - radius));
  const int x_max = static_cast<int>(std::floor(center.x + radius));
  const int y_min = static_cast<int>(std::ceil(center.y - radius));
  const int y_max = static_cast<int>(std::floor(center.y + radius));

  // Step 2: 各ノードについて掘削を行う。
  for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
      // ノード位置と中心からの距離を計算
      const Point node_pos { x, y };
      const double dist = center.distanceFrom(node_pos);
      if (not node_grid_.IsBound(node_pos)) continue;
      if (dist > radius) continue;

      // 掘削量を計算
      const double t = dist / radius;
      const double dig_might = center_might * (1.0 - t) + end_might * t;

      // ノードの値を更新
      double current_value = node_grid_.Get(node_pos);
      current_value += dig_might;
      current_value = std::clamp(current_value, 0.0, 1.0);
      node_grid_.Set(node_pos, current_value);

      // 端の場合は常に0とする
      if (x == 0 || y == 0 || x == static_cast<int>(node_grid_.GetSize().x) - 1 || y == static_cast<int>(node_grid_.GetSize().y) - 1) {
        node_grid_.Set(node_pos, 0.0);
      }
    }
  }

}

void Terrain::RenderGround(const Array<Point>& visible_cells, const camera::Camera& cam) const {

  {
    const ScopedRenderStates2D blend { render::BlendMode::AlphaMax() };
    const auto t = cam.CreateRenderTransformer();

    const auto& polygons = marching_squares_.GetPolygons();
    for (const auto& pos : visible_cells) {
      const auto cell_polygons = polygons.Get(pos);
      for (const auto& polygon : cell_polygons) {
        polygon.draw(ColorF { 0.1, 0.1, 0.30 });
      }
    }
  }
}

void Terrain::RenderGroundEdges(const Array<Point>& visible_cells, const camera::Camera& cam) const {
  const auto& lightbloom = render::LightBloom::GetInstance();

  // 通常の線を描画
  {
    const auto target = lightbloom.CreateRenderTarget();
    const auto transform = cam.CreateRenderTransformer();
    const auto blend { render::BlendMode::AlphaMax() };

    const auto& lines = marching_squares_.GetEdgeLines();
    for (const auto& pos : visible_cells) {
      const auto edge_lines = lines.Get(pos);
      for (const auto& vec_array : edge_lines) {
        for (size_t i = 0; i < vec_array.size() - 1; ++i) {
          Line { vec_array[i], vec_array[i + 1] }.draw(LineStyle::RoundCap, 0.3, ColorF { 0.2, 0.3, 1.0 });
        }
      }
    }
  }

  // ブラー処理
  lightbloom.Apply(1.0, 2.0, 2.0);
}


Array<Point> Terrain::CreateVisibleCells(const camera::Camera& cam) const {
  const Vec2& center = cam.GetCenter();
  const SizeF& cell_size = cam.GetCellSize();

  int begin_index_x = static_cast<int>(std::floor(center.x - Scene::Width() / (2.0 * cell_size.x)));
  int begin_index_y = static_cast<int>(std::floor(center.y - Scene::Height() / (2.0 * cell_size.y)));
  int index_count_x = static_cast<int>(std::ceil(Scene::Width() / cell_size.x)) + 1;
  int index_count_y = static_cast<int>(std::ceil(Scene::Height() / cell_size.y)) + 1;

  index_count_x = std::min(index_count_x, static_cast<int>(GetCellGridSize().x) - begin_index_x);
  index_count_y = std::min(index_count_y, static_cast<int>(GetCellGridSize().y) - begin_index_y);
  Array<Point> visible_cells { Arg::reserve = index_count_y * index_count_x };
  for (int y = 0; y <= index_count_y; ++y) {
    for (int x = 0; x < index_count_x; ++x) {
      visible_cells.emplace_back(begin_index_x + x, begin_index_y + y);
    }
  }
  return visible_cells;
}

}

