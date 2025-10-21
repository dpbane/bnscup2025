#include "stdafx.h"
#include "terrain.hpp"

#include <ranges>

namespace bnscup2025::terrain {

constexpr int kBlurRate = 4;

Terrain::Terrain(NodeGrid node_grid) :
  node_grid_(node_grid),
  marching_squares_(node_grid_, 0.5),
  temp_texture_(Scene::Size()),
  blur_texture_(Scene::Size() / kBlurRate),
  blur_internal_texture_(Scene::Size() / kBlurRate) {
}

void Terrain::Update() {
  marching_squares_.Update(node_grid_);
}

void Terrain::Render(const Vec2& center, const SizeF& cell_size) const {

  // 描画範囲セルの取得
  Array<Point> visible_cells = CreateVisibleCells(center, cell_size);

  // 各種描画
  RenderGround(visible_cells, center, cell_size);
  RenderGroundEdges(visible_cells, center, cell_size);
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

Transformer2D Terrain::CreateRenderTransformer(const Vec2& center, const SizeF& cell_size) const {
  Vec2 offset = -(center - Scene::Size() / (2.0 * cell_size)) * cell_size;
  Transformer2D t { Mat3x2::Scale(cell_size).translated(offset) };
  return t;
}

void Terrain::RenderGround(const Array<Point>& visible_cells, const Vec2& center, const SizeF& cell_size) const {

  {
    const ScopedRenderTarget2D target { temp_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 0.0 }) };
    const ScopedRenderStates2D blend { MakeBlendState() };
    const auto t = CreateRenderTransformer(center, cell_size);

    const auto& polygons = marching_squares_.GetPolygons();
    for (const auto& pos : visible_cells) {
      const auto cell_polygons = polygons.Get(pos);
      for (const auto& polygon : cell_polygons) {
        polygon.draw(ColorF { 0.1, 0.1, 0.15 });
      }
    }
  }

  Graphics2D::Flush();
  temp_texture_.resolve();
  temp_texture_.draw();
}

void Terrain::RenderGroundEdges(const Array<Point>& visible_cells, const Vec2& center, const SizeF& cell_size) const {
  // 通常の線を描画
  {
    const ScopedRenderTarget2D target { temp_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 0.0 }) };
    const ScopedRenderStates2D blend { MakeBlendState() };
    const auto t = CreateRenderTransformer(center, cell_size);

    const auto& lines = marching_squares_.GetEdgeLines();
    for (const auto& pos : visible_cells) {
      const auto edge_lines = lines.Get(pos);
      for (const auto& vec_array : edge_lines) {
        for (size_t i = 0; i < vec_array.size() - 1; ++i) {
          Line { vec_array[i], vec_array[i + 1] }.draw(LineStyle::RoundCap, 0.1, ColorF { 0.2, 0.2, 1.0 });
        }
      }
    }
  }

  // ブラー処理
  Graphics2D::Flush();
  temp_texture_.resolve();
  Shader::Downsample(temp_texture_, blur_texture_);
  Shader::GaussianBlur(blur_texture_, blur_internal_texture_, blur_texture_);

  // 描画
  {
    const ScopedRenderStates2D blend { BlendState::Additive };
    temp_texture_.draw();
    blur_texture_.resized(Scene::Size()).draw();
  }
}


Array<Point> Terrain::CreateVisibleCells(Vec2 center, SizeF cell_size) const {
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

BlendState Terrain::MakeBlendState() const {
  BlendState blendState = BlendState::Default2D;
  blendState.srcAlpha = Blend::SrcAlpha;
  blendState.dstAlpha = Blend::DestAlpha;
  blendState.opAlpha = BlendOp::Max;
  return blendState;
}




}
