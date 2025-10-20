#pragma once

#include "marching_squares.hpp"

namespace bnscup2025::terrain {

class Terrain {
public:
  Terrain(NodeGrid node_grid);

  /// @brief 地形を更新する。
  void Update();

  /// @brief 地形を描画する。
  /// @param center 画面の中心となるセル座標。(0, 0)が左上のセルの左上角に対応する。
  /// @param cell_size セル1つが何ピクセルになるか。
  void Render(const Vec2& center, const SizeF& cell_size) const;

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
  /// @param center 画面の中心となるセル座標。(0, 0)が左上のセルの左上角に対応する。
  /// @param cell_size セル1つが何ピクセルになるか。
  /// @return 線分の配列。
  Array<Line> CreateVisibleWallLines(const Vec2& center, const SizeF& cell_size) const;

private:
  /// @brief 地面を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  void RenderGround(const Array<Point>& visible_cells) const;

  /// @brief 地面の境界を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  void RenderGroundEdges(const Array<Point>& visible_cells) const;

  /// @brief 壁を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param wall_height 壁の高さ。cell_size.yに対する比率で指定する。
  /// @attention 廃止。真上から見たほうが視野制限処理がやりやすい。
  void RenderWalls(const Array<Point>& visible_cells, double wall_height) const;

  /// @brief 壁上端のエッジを描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param wall_height 壁の高さ。cell_size.yに対する比率で指定する。
  /// @attention 廃止。真上から見たほうが視野制限処理がやりやすい。
  void RenderWallTopEdges(const Array<Point>& visible_cells, double wall_height) const;

private:
  /// @brief 描画範囲内のセルの位置の配列を作成する。
  /// @param center 画面の中心となるセル座標。(0, 0)が左上のセルの左上角に対応する。
  /// @param cell_size セル1つが何ピクセルになるか。
  /// @return 描画範囲内のセルの位置の配列
  Array<Point> CreateVisibleCells(Vec2 center, SizeF cell_size) const;

private:
  NodeGrid node_grid_;
  MarchingSquares marching_squares_;

  RenderTexture ground_texture_;


};

}
