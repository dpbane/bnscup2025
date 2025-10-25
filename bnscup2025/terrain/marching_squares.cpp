#include "stdafx.h"
#include "marching_squares.hpp"

namespace bnscup2025::terrain {

MarchingSquares::MarchingSquares(const NodeGrid& node_grid, double threshold) :
  node_grid_(node_grid),
  case_grid_(node_grid.GetSize() - Size(1, 1)),
  edge_line_grid_(node_grid.GetSize() - Size(1, 1)),
  polygon_grid_(node_grid.GetSize() - Size(1, 1)),
  threshold_(threshold) {

  // 全てのセルを更新する
  GridPoints<bool> update_grid(node_grid.GetSize() - Size(1, 1));
  for (size_t y = 0; y < update_grid.GetSize().y; ++y) {
    for (size_t x = 0; x < update_grid.GetSize().x; ++x) {
      const Point pos(static_cast<int>(x), static_cast<int>(y));
      update_grid.Set(pos, true);
    }
  }
  UpdateCell(update_grid);
}

void MarchingSquares::Update(const NodeGrid& node_grid) {
  const GridPoints<bool> update_grid = CreateUpdateGrid(node_grid);
  node_grid_ = node_grid;
  UpdateCell(update_grid);
}

GridPoints<bool> MarchingSquares::CreateUpdateGrid(const NodeGrid& node_grid) {
  GridPoints<bool> ret(node_grid.GetSize() - Size(1, 1));

  for (size_t y = 0; y < node_grid.GetSize().y; ++y) {
    for (size_t x = 0; x < node_grid.GetSize().x; ++x) {
      const Point point(static_cast<int>(x), static_cast<int>(y));
      const double prev_value = node_grid_.Get(point).density;
      const double curr_value = node_grid.Get(point).density;
      if (prev_value != curr_value) {
        ret.Set(point.movedBy(0, 0), true);
        ret.Set(point.movedBy(0, -1), true);
        ret.Set(point.movedBy(-1, 0), true);
        ret.Set(point.movedBy(-1, -1), true);
      }
    }
  }

  return ret;
}

void MarchingSquares::UpdateCell(const GridPoints<bool>& update_grid) {

  /// @brief 指定した位置のセルのケースを更新する。
  auto UpdateCase = [this](const Point& pos) {
    const Case case_value = CalcCellCase(pos);
    case_grid_.Set(pos, case_value);
  };

  /// @brief 指定した位置のセルのエッジラインとポリゴンを更新する。
  auto UpdateEdgeLinesAndPolygons = [this](const Point& pos) {
    const auto [edge_lines, polygons] = CalcEdgeLinesAndPolygons(pos);
    edge_line_grid_.Set(pos, edge_lines);
    polygon_grid_.Set(pos, polygons);
  };


  // 更新が必要なすべてのセルを更新する。
  for (size_t y = 0; y < case_grid_.GetSize().y; ++y) {
    for (size_t x = 0; x < case_grid_.GetSize().x; ++x) {
      const Point pos(static_cast<int>(x), static_cast<int>(y));
      if (not update_grid.Get(pos)) continue;

      UpdateCase(pos);
      UpdateEdgeLinesAndPolygons(pos);
    }
  }
}

MarchingSquares::Case MarchingSquares::CalcCellCase(const Point& pos) {
  const bool tl = node_grid_.Get(pos.movedBy(0, 0)).density >= threshold_;
  const bool tr = node_grid_.Get(pos.movedBy(1, 0)).density >= threshold_;
  const bool bl = node_grid_.Get(pos.movedBy(0, 1)).density >= threshold_;
  const bool br = node_grid_.Get(pos.movedBy(1, 1)).density >= threshold_;
  const bool center = (node_grid_.Get(pos.movedBy(0, 0)).density
    + node_grid_.Get(pos.movedBy(1, 0)).density
    + node_grid_.Get(pos.movedBy(0, 1)).density
    + node_grid_.Get(pos.movedBy(1, 1)).density) >= (4.0 * threshold_);
  uint8 index = 0;
  if (tl) index |= 0b0001;
  if (tr) index |= 0b0010;
  if (br) index |= 0b0100;
  if (bl) index |= 0b1000;

  switch (index) {
    case 0b0000: return Case::None;
    case 0b1111: return Case::Full;
    case 0b0001: return Case::TopLeft;
    case 0b0010: return Case::TopRight;
    case 0b0100: return Case::BottomRight;
    case 0b1000: return Case::BottomLeft;
    case 0b0011: return Case::Top;
    case 0b1100: return Case::Bottom;
    case 0b1001: return Case::Left;
    case 0b0110: return Case::Right;
    case 0b1110: return Case::InvertedTopLeft;
    case 0b1101: return Case::InvertedTopRight;
    case 0b1011: return Case::InvertedBottomRight;
    case 0b0111: return Case::InvertedBottomLeft;
    case 0b1010:
      if (center) return Case::SaddleRisingConnected;
      return Case::SaddleRisingSeparated;
    case 0b0101:
      if (center) return Case::SaddleFallingConnected;
      return Case::SaddleFallingSeparated;
    default: assert(false); return Case::None;
  }
}

std::pair<Array<Array<Vec2>>, Array<Polygon>> MarchingSquares::CalcEdgeLinesAndPolygons(const Point& pos) {
  std::pair<Array<Array<Vec2>>, Array<Polygon>> ret;

  // 基本となる値
  const double tl = node_grid_.Get(pos.movedBy(0, 0)).density;
  const double tr = node_grid_.Get(pos.movedBy(1, 0)).density;
  const double bl = node_grid_.Get(pos.movedBy(0, 1)).density;
  const double br = node_grid_.Get(pos.movedBy(1, 1)).density;
  const double center = (tl + tr + bl + br) / 4.0;
  const Case case_value = case_grid_.Get(pos);

  // 媒介変数tを求める関数
  auto CalcT = [this](double v1, double v2) -> double {
    if (v1 == v2) return 0.5;
    return (threshold_ - v1) / (v2 - v1);
  };

  // 各種点たち
  const Vec2 p_tl = pos.movedBy(0, 0);
  const Vec2 p_tr = pos.movedBy(1, 0);
  const Vec2 p_bl = pos.movedBy(0, 1);
  const Vec2 p_br = pos.movedBy(1, 1);
  const Vec2 p_top = p_tl.lerp(p_tr, CalcT(tl, tr));
  const Vec2 p_bottom = p_bl.lerp(p_br, CalcT(bl, br));
  const Vec2 p_left = p_tl.lerp(p_bl, CalcT(tl, bl));
  const Vec2 p_right = p_tr.lerp(p_br, CalcT(tr, br));
  const Vec2 p_center = pos + Vec2(0.5, 0.5);
  const Vec2 p_center_tl = p_center.lerp(p_tl, CalcT(center, tl));
  const Vec2 p_center_tr = p_center.lerp(p_tr, CalcT(center, tr));
  const Vec2 p_center_bl = p_center.lerp(p_bl, CalcT(center, bl));
  const Vec2 p_center_br = p_center.lerp(p_br, CalcT(center, br));

  // エイリアス
  auto& edge_lines = ret.first;
  auto& polygons = ret.second;


  // エッジラインとポリゴンをケースごとに計算
  // エッジラインは正の領域が時計回りに囲まれる順で格納する。
  // ポリゴンは多角形が時計回りになるように格納する。
  switch (case_value) {
    case Case::None:
      break;

    case Case::Full:
      polygons.emplace_back(Array<Vec2>{ p_tl, p_tr, p_br, p_bl }.uniqued_consecutive());
      break;

    case Case::TopLeft:
      edge_lines.push_back({ p_top, p_left });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_top, p_left }.uniqued_consecutive());
      break;

    case Case::TopRight:
      edge_lines.push_back({ p_right, p_top });
      polygons.emplace_back(Array<Vec2>{ p_tr, p_right, p_top}.uniqued_consecutive());
      break;

    case Case::BottomLeft:
      edge_lines.push_back({ p_left, p_bottom });
      polygons.emplace_back(Array<Vec2>{ p_bl, p_left, p_bottom}.uniqued_consecutive());
      break;

    case Case::BottomRight:
      edge_lines.push_back({ p_bottom, p_right });
      polygons.emplace_back(Array<Vec2>{ p_br, p_bottom, p_right}.uniqued_consecutive());
      break;

    case Case::Top:
      edge_lines.push_back({ p_right, p_left });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_tr, p_right, p_left}.uniqued_consecutive());
      break;

    case Case::Bottom:
      edge_lines.push_back({ p_left, p_right });
      polygons.emplace_back(Array<Vec2>{ p_bl, p_left, p_right, p_br}.uniqued_consecutive());
      break;

    case Case::Left:
      edge_lines.push_back({ p_top, p_bottom });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_top, p_bottom, p_bl}.uniqued_consecutive());
      break;

    case Case::Right:
      edge_lines.push_back({ p_bottom, p_top });
      polygons.emplace_back(Array<Vec2>{ p_tr, p_br, p_bottom, p_top}.uniqued_consecutive());
      break;

    case Case::InvertedTopLeft:
      edge_lines.push_back({ p_left, p_top });
      polygons.emplace_back(Array<Vec2>{ p_left, p_top, p_tr, p_br, p_bl}.uniqued_consecutive());
      break;

    case Case::InvertedTopRight:
      edge_lines.push_back({ p_top, p_right });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_top, p_right, p_br, p_bl}.uniqued_consecutive());
      break;

    case Case::InvertedBottomLeft:
      edge_lines.push_back({ p_bottom, p_left });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_tr, p_br, p_bottom, p_left}.uniqued_consecutive());
      break;

    case Case::InvertedBottomRight:
      edge_lines.push_back({ p_right, p_bottom });
      polygons.emplace_back(Array<Vec2>{ p_tl, p_tr, p_right, p_bottom, p_bl}.uniqued_consecutive());
      break;

    case Case::SaddleRisingConnected:
      edge_lines.push_back({ p_left, p_center_tl, p_top });
      edge_lines.push_back({ p_right, p_center_br, p_bottom });
      polygons.emplace_back(Array<Vec2>{ p_left, p_center_tl, p_top, p_tr, p_right, p_center_br, p_bottom, p_bl }.uniqued_consecutive());
      break;

    case Case::SaddleRisingSeparated:
      edge_lines.push_back({ p_left, p_center_bl, p_bottom });
      edge_lines.push_back({ p_right, p_center_tr, p_top });
      polygons.emplace_back(Array<Vec2>{ p_left, p_center_bl, p_bottom, p_bl }.uniqued_consecutive());
      polygons.emplace_back(Array<Vec2>{ p_right, p_center_tr, p_top, p_tr }.uniqued_consecutive());
      break;

    case Case::SaddleFallingConnected:
      edge_lines.push_back({ p_top, p_center_tr, p_right });
      edge_lines.push_back({ p_bottom, p_center_bl, p_left });
      polygons.emplace_back(Array<Vec2>{ p_top, p_center_tr, p_right, p_br, p_bottom, p_center_bl, p_left, p_tl }.uniqued_consecutive());
      break;

    case Case::SaddleFallingSeparated:
      edge_lines.push_back({ p_top, p_center_tl, p_left });
      edge_lines.push_back({ p_bottom, p_center_br, p_right });
      polygons.emplace_back(Array<Vec2>{ p_top, p_center_tl, p_left, p_tl }.uniqued_consecutive());
      polygons.emplace_back(Array<Vec2>{ p_bottom, p_center_br, p_right, p_br }.uniqued_consecutive());
      break;

    default:
      assert(false);
      break;
  }

  return ret;
}




}
