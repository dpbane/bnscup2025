#pragma once

#include "node_grid.hpp"

namespace bnscup2025::terrain {

class MarchingSquares {
public:
  enum class Case : uint8 {
    None,
    Full,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Top,
    Bottom,
    Left,
    Right,
    InvertedTopLeft,
    InvertedTopRight,
    InvertedBottomLeft,
    InvertedBottomRight,
    SaddleRisingConnected,
    SaddleRisingSeparated,
    SaddleFallingConnected,
    SaddleFallingSeparated
  };

public:
  /// @brief コンストラクタ。
  /// @param node_grid 頂点グリッド。
  /// @param threshold 閾値。
  MarchingSquares(const NodeGrid& node_grid, double threshold = 0.5);

  /// @brief 頂点グリッドの変更に応じてセルを更新する。
  /// @param update_node 更新が必要なセルはtrue、それ以外はfalseのグリッド。
  void Update(const GridPoints<bool>& update_node);

  /// @brief 各セルに対応するケースのグリッドを取得する。
  /// @return ケースのグリッド。
  const GridPoints<Case>& GetCaseGrid() const { return case_grid_; }

  /// @brief 各セルに対応するエッジラインのグリッドを取得する。
  /// @return エッジラインのグリッド。
  const GridPoints<Array<Array<Vec2>>>& GetEdgeLines() const { return edge_line_grid_; }

  /// @brief 各セルに対応するポリゴンのグリッドを取得する。
  /// @return ポリゴンのグリッド。
  const GridPoints<Array<Polygon>>& GetPolygons() const { return polygon_grid_; }

private:
  /// @brief セルを更新する。
  /// @param update_node 更新が必要なセルはtrue、それ以外はfalseのグリッド。
  void UpdateCell(const GridPoints<bool>& update_cell);

private:
  /// @brief あるセルのケースを取得する。
  /// @param pos セルの位置
  /// @return セルのケース。
  Case CalcCellCase(const Point& pos);

  /// @brief あるセルのエッジラインとポリゴンを計算する。
  /// @param pos セルの位置
  /// @return セルのエッジライン。
  std::pair<Array<Array<Vec2>>, Array<Polygon>> CalcEdgeLinesAndPolygons(const Point& pos);

private:
  const NodeGrid& node_grid_;
  GridPoints<Case> case_grid_;
  GridPoints<Array<Array<Vec2>>> edge_line_grid_;
  GridPoints<Array<Polygon>> polygon_grid_;
  double threshold_;

};

}
