#pragma once

#include "marching_squares.hpp"

#include "camera/camera.hpp"

namespace bnscup2025::terrain {

class Terrain {
public:
  Terrain(NodeGrid node_grid);

  /// @brief 地形を更新する。
  void Update();

  /// @brief 地形を描画する。
  /// @param cam カメラ。
  void Render(const camera::Camera& cam) const;

  /// @brief 指定した位置のノードの値を取得する。
  /// @param pos ノードの位置
  /// @return ノードの値
  double GetNodeValue(Point pos) const;

  /// @brief 指定した位置のノードの値を変更する。
  /// @param pos ノードの位置
  /// @param value 新しいノードの値
  void ModifyNode(Point pos, double value);

  /// @brief ノードグリッドのサイズを取得する。
  /// @return ノードグリッドのサイズ
  Size GetNodeGridSize() const;

  /// @brief セルグリッドのサイズを取得する。
  /// @return セルグリッドのサイズ
  Size GetCellGridSize() const;

  /// @brief 画面に描画される範囲の壁を表す線分の配列を取得する。
  /// @param cam カメラ。
  /// @return 線分の配列。
  Array<Line> CreateVisibleWallLines(const camera::Camera& cam) const;

private:
  /// @brief 地面を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param cam カメラ。
  void RenderGround(const Array<Point>& visible_cells, const camera::Camera& cam) const;

  /// @brief 地面の境界を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param cam カメラ。
  void RenderGroundEdges(const Array<Point>& visible_cells, const camera::Camera& cam) const;

private:
  /// @brief 描画範囲内のセルの位置の配列を作成する。
  /// @param cam カメラ。
  /// @return 描画範囲内のセルの位置の配列
  Array<Point> CreateVisibleCells(const camera::Camera& cam) const;

private:
  NodeGrid node_grid_;
  MarchingSquares marching_squares_;

};

}
