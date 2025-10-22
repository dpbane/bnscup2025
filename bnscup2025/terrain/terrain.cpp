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

double Terrain::GetNodeValue(Point pos) const {
  return node_grid_.Get(pos);
}

void Terrain::ModifyNode(Point pos, double value) {
  node_grid_.Set(pos, value);
}

Size Terrain::GetNodeGridSize() const {
  return node_grid_.GetSize();
}

Size Terrain::GetCellGridSize() const {
  return marching_squares_.GetPolygons().GetSize();
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

void Terrain::RenderGround(const Array<Point>& visible_cells, const camera::Camera& cam) const {

  {
    const ScopedRenderStates2D blend { render::BlendMode::AlphaMax() };
    const auto t = cam.CreateRenderTransformer();

    const auto& polygons = marching_squares_.GetPolygons();
    for (const auto& pos : visible_cells) {
      const auto cell_polygons = polygons.Get(pos);
      for (const auto& polygon : cell_polygons) {
        polygon.draw(ColorF { 0.1, 0.1, 0.15 });
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
          Line { vec_array[i], vec_array[i + 1] }.draw(LineStyle::RoundCap, 0.2, ColorF { 0.2, 0.2, 1.0 });
        }
      }
    }
  }

  // ブラー処理
  lightbloom.Apply(1.0, 1.0, 1.0);
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

