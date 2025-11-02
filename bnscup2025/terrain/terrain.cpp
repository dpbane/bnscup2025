#include "stdafx.h"
#include "terrain.hpp"

#include <ranges>

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"

#include "pushback_service.hpp"

namespace bnscup2025::terrain {

constexpr int kBlurRate = 4;
constexpr double kThreshold = 0.5;

Terrain::Terrain(NodeGrid node_grid) :
  node_grid_(node_grid),
  marching_squares_(node_grid_, kThreshold),
  access_map_(node_grid_, marching_squares_, kThreshold),
  updated_node_(node_grid_.GetSize()) {

  material_table_[MaterialEnum::Normal] = std::make_unique<Material<MaterialEnum::Normal>>();
  material_table_[MaterialEnum::Bounds] = std::make_unique<Material<MaterialEnum::Bounds>>();
  material_table_[MaterialEnum::HardRock] = std::make_unique<Material<MaterialEnum::HardRock>>();

  updated_node_.Fill(true);
  Update();
}

Terrain::Terrain(Terrain&& terrain) noexcept :
  node_grid_(std::move(terrain.node_grid_)),
  sinhalite_positions_(std::move(terrain.sinhalite_positions_)),
  marching_squares_(node_grid_, kThreshold),
  access_map_(node_grid_, marching_squares_, kThreshold),
  updated_node_(node_grid_.GetSize()) {

  material_table_[MaterialEnum::Normal] = std::make_unique<Material<MaterialEnum::Normal>>();
  material_table_[MaterialEnum::Bounds] = std::make_unique<Material<MaterialEnum::Bounds>>();
  material_table_[MaterialEnum::HardRock] = std::make_unique<Material<MaterialEnum::HardRock>>();

  updated_node_.Fill(true);
  Update();
}

void Terrain::Update() {
  marching_squares_.Update(updated_node_);
  access_map_.Update(updated_node_);

  earned_sinhalite_ = 0;
  updated_node_.Fill(false);
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

void Terrain::SetSinhalitePositions(Array<Point>&& positions) {
  sinhalite_positions_ = std::move(positions);
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

Vec2 Terrain::Pushback(const Circle& circle) const {
  return PushbackService::Exec(marching_squares_, circle);
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
      const double dig_might = (center_might * (1.0 - t) + end_might * t) * material_table_.at(node_grid_.Get(node_pos).material)->GetDiggability();

      // ノードの値を更新
      double current_value = node_grid_.Get(node_pos).density;
      current_value += dig_might;
      current_value = std::clamp(current_value, 0.0, 1.0);
      node_grid_.Set(node_pos, NodeInfo { .density = current_value, .material = node_grid_.Get(node_pos).material });

      updated_node_.Set(node_pos, true);
      updated_node_.Set(node_pos.movedBy(-1, 0), true);
      updated_node_.Set(node_pos.movedBy(1, 0), true);
      updated_node_.Set(node_pos.movedBy(0, -1), true);
      updated_node_.Set(node_pos.movedBy(0, 1), true);
    }
  }
  EarnSinhalite();
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

      // デバッグ用: アクセスマップの表示
      if (false) {
        if (access_map_.GetAccessableMap().Get(pos)) {
          Circle { pos, 0.1 }.draw(ColorF { 1, 0, 0 });

          const auto direction = access_map_.GetDirectionMap().Get(pos);
          if (direction.up) Line { Vec2{pos}, Vec2{pos}.movedBy(0, -0.4) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.down) Line { Vec2{pos}, Vec2{pos}.movedBy(0, 0.4) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.left) Line { Vec2{pos}, Vec2{pos}.movedBy(-0.4, 0) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.right) Line { Vec2{pos}, Vec2{pos}.movedBy(0.4, 0) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.up_left) Line { Vec2{pos}, Vec2{pos}.movedBy(-0.4, -0.4) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.up_right) Line { Vec2{pos}, Vec2{pos}.movedBy(0.4, -0.4) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.down_left) Line { Vec2{pos}, Vec2{pos}.movedBy(-0.4, 0.4) }.draw(0.1, ColorF { 1, 0, 0 });
          if (direction.down_right) Line { Vec2{pos}, Vec2{pos}.movedBy(0.4, 0.4) }.draw(0.1, ColorF { 1, 0, 0 });

        }
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
          const Line line { vec_array[i], vec_array[i + 1] };
          line.draw(LineStyle::RoundCap, 0.8, CalcEdgeColor(line.begin), CalcEdgeColor(line.end));
        }
      }
    }
  }

  // ブラー処理
  lightbloom.Apply(1.0, 3.0, 3.0);
}

ColorF Terrain::CalcEdgeColor(const Vec2& pos) const {
  int x_i = static_cast<int>(std::floor(pos.x));
  int y_i = static_cast<int>(std::floor(pos.y));
  double x_frac = pos.x - x_i;
  double y_frac = pos.y - y_i;
  const Point cell_pos { x_i, y_i };
  const auto color_lt = material_table_.at(node_grid_.Get(cell_pos.movedBy(0, 0)).material)->GetBaseColor();
  const auto color_rt = material_table_.at(node_grid_.Get(cell_pos.movedBy(1, 0)).material)->GetBaseColor();
  const auto color_lb = material_table_.at(node_grid_.Get(cell_pos.movedBy(0, 1)).material)->GetBaseColor();
  const auto color_rb = material_table_.at(node_grid_.Get(cell_pos.movedBy(1, 1)).material)->GetBaseColor();
  const double coef_top = 1.0 - y_frac;
  const double coef_bottom = y_frac;
  const double coef_left = 1.0 - x_frac;
  const double coef_right = x_frac;
  const double coef_lt = coef_top * coef_left;
  const double coef_rt = coef_top * coef_right;
  const double coef_lb = coef_bottom * coef_left;
  const double coef_rb = coef_bottom * coef_right;
  const ColorF color_base = color_lt * coef_lt + color_rt * coef_rt + color_lb * coef_lb + color_rb * coef_rb;

  double sinhalite_intensity = 0.0;
  constexpr double kSinhaliteEffectRadius = 34.0;
  for (const auto& sinhalite_pos : sinhalite_positions_) {
    const double dist = pos.distanceFrom(sinhalite_pos);
    const double intensity = std::clamp(1.0 - dist / kSinhaliteEffectRadius, 0.0, 1.0);
    sinhalite_intensity = std::max(sinhalite_intensity, intensity);
  }
  const double int_lt = coef_lt * material_table_.at(node_grid_.Get(cell_pos.movedBy(0, 0)).material)->GetSinhaliteEffectiveness();
  const double int_rt = coef_rt * material_table_.at(node_grid_.Get(cell_pos.movedBy(1, 0)).material)->GetSinhaliteEffectiveness();
  const double int_lb = coef_lb * material_table_.at(node_grid_.Get(cell_pos.movedBy(0, 1)).material)->GetSinhaliteEffectiveness();
  const double int_rb = coef_rb * material_table_.at(node_grid_.Get(cell_pos.movedBy(1, 1)).material)->GetSinhaliteEffectiveness();
  const double sinhalite_effectiveness = int_lt + int_rt + int_lb + int_rb;

  constexpr ColorF kSinhaliteColor { 0.25, 0.25, 0.01 };
  return color_base.lerp(kSinhaliteColor, EaseInCubic(sinhalite_intensity * sinhalite_effectiveness));
}

void Terrain::EarnSinhalite() {
  earned_sinhalite_ = 0;

  for (auto itr = sinhalite_positions_.begin(); itr != sinhalite_positions_.end(); ) {
    const Point pos = *itr;
    const auto node_info = node_grid_.Get(pos);
    if (node_info.density > kThreshold) {
      ++earned_sinhalite_;
      itr = sinhalite_positions_.erase(itr);
    }
    else {
      ++itr;
    }
  }
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

