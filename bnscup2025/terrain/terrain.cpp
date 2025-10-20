#include "stdafx.h"
#include "terrain.hpp"

#include <ranges>

namespace bnscup2025::terrain {

Terrain::Terrain(NodeGrid node_grid) :
  node_grid_(node_grid),
  marching_squares_(node_grid_, 0.2),
  ground_texture_(Scene::Size()) {
}

void Terrain::Update() {
  marching_squares_.Update(node_grid_);
}

void Terrain::Render(const Vec2& center, const SizeF& cell_size) const {

  // 描画範囲セルの取得
  Array<Point> visible_cells = CreateVisibleCells(center, cell_size);

  // 描画用変換
  Vec2 offset = -(center - Scene::Size() / (2.0 * cell_size)) * cell_size;
  const Transformer2D t1 { Mat3x2::Translate(offset) };
  const Transformer2D t2 { Mat3x2::Scale(cell_size) };

  // 各種描画
  RenderGround(visible_cells);
  RenderGroundEdges(visible_cells);
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

Array<Line> Terrain::CreateVisibleWallLines(const Vec2& center, const SizeF& cell_size) const {
  Array<Point> visible_cells = CreateVisibleCells(center, cell_size);
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

void Terrain::RenderGround(const Array<Point>& visible_cells) const {
  const auto& polygons = marching_squares_.GetPolygons();
  for (const auto& pos : visible_cells) {
    const auto cell_polygons = polygons.Get(pos);
    for (const auto& polygon : cell_polygons) {
      polygon.draw(Palette::Darkgray);
    }
  }
}

void Terrain::RenderGroundEdges(const Array<Point>& visible_cells) const {
  const auto& lines = marching_squares_.GetEdgeLines();
  for (const auto& pos : visible_cells) {
    const auto edge_lines = lines.Get(pos);
    for (const auto& vec_array : edge_lines) {
      for (size_t i = 0; i < vec_array.size() - 1; ++i) {
        Line { vec_array[i], vec_array[i + 1] }.draw(LineStyle::RoundCap, 0.1, Palette::Black);
      }
    }
  }
}

void Terrain::RenderWalls(const Array<Point>& visible_cells, double wall_height) const {
  const auto& lines = marching_squares_.GetEdgeLines();
  for (const auto& pos : visible_cells) {
    const auto edge_lines = lines.Get(pos);
    for (const auto& vec_array : edge_lines) {
      for (size_t i = 0; i < vec_array.size() - 1; ++i) {
        if (vec_array[i + 1].x < vec_array[i].x) continue;  // 奥側の壁のみを描画する
        Polygon {
          vec_array[i + 1],
          vec_array[i],
          vec_array[i] + Vec2{ 0.0, -wall_height },
          vec_array[i + 1] + Vec2{ 0.0, -wall_height }
        }.draw(Palette::Brown);
      }
    }
  }
}

void Terrain::RenderWallTopEdges(const Array<Point>& visible_cells, double wall_height) const {
  const auto& lines = marching_squares_.GetEdgeLines();
  for (const auto& pos : visible_cells) {
    const auto edge_lines = lines.Get(pos);
    for (const auto& vec_array : edge_lines) {
      for (size_t i = 0; i < vec_array.size() - 1; ++i) {
        //if (vec_array[i + 1].x < vec_array[i].x) continue;  // 奥側の壁のみを描画する
        Line {
          vec_array[i] + Vec2{ 0.0, -wall_height },
          vec_array[i + 1] + Vec2{ 0.0, -wall_height }
        }.draw(LineStyle::RoundCap, 0.1, Palette::Blue);
      }
    }
  }
}

Array<Point> Terrain::CreateVisibleCells(Vec2 center, SizeF cell_size) const {
  int begin_index_x = static_cast<int>(std::floor(center.x - Scene::Width() / (2.0 * cell_size.x)));
  int begin_index_y = static_cast<int>(std::floor(center.y - Scene::Height() / (2.0 * cell_size.y)));
  int index_count_x = static_cast<int>(std::ceil(Scene::Width() / cell_size.x)) + 1;
  int index_count_y = static_cast<int>(std::ceil(Scene::Height() / cell_size.y)) + 1;

  index_count_x = std::min(index_count_x, static_cast<int>(GetCellGridSize().x) - begin_index_x);
  index_count_y = std::min(index_count_y, static_cast<int>(GetCellGridSize().y) - begin_index_y);
  Array<Point> visible_cells(index_count_y * index_count_x);
  for (int y = 0; y <= index_count_y; ++y) {
    for (int x = 0; x < index_count_x; ++x) {
      visible_cells.emplace_back(begin_index_x + x, begin_index_y + y);
    }
  }
  return visible_cells;
}




}
