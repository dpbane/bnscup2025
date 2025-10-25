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

  /// @brief ノードグリッドのサイズを取得する。
  /// @return ノードグリッドのサイズ
  Size GetNodeGridSize() const;

  /// @brief セルグリッドのサイズを取得する。
  /// @return セルグリッドのサイズ
  Size GetCellGridSize() const;

  /// @brief 指定した位置に対応するケースを取得する。
  /// @param pos 取得する位置。
  /// @return 位置に対応するケース
  MarchingSquares::Case GetCase(const Point& pos) const;

  /// @brief 画面に描画される範囲の壁を表す線分の配列を取得する。
  /// @param cam カメラ。
  /// @return 線分の配列。
  Array<Line> CreateVisibleWallLines(const camera::Camera& cam) const;

  /// @brief プレイヤーや敵が壁にめり込まないための押し戻し量を計算する。
  /// @param circle 押し戻しを適用する円。
  /// @return 押し戻しを適用した後の中心位置。
  Vec2 PushbackService(const Circle& circle) const;

  /// @brief ある地点から直線を伸ばしたとき、最初に壁に衝突する位置を計算する。
  /// @return 衝突位置。衝突しない場合は無効値を返す。
  Optional<Vec2> CalcLineCollisionPoint(const Vec2& from, const Vec2& direction, double max_distance) const;

  /// @brief 指定した位置を掘削する。
  /// @param center 掘削中心位置。
  /// @param radius 掘削半径。
  /// @param center_might 中心の掘削量。
  /// @param end_might 端の掘削量。間は線形補完される。
  void DigAt(const Vec2& center, double radius, double center_might, double end_might);

private:
  /// @brief 地面を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param cam カメラ。
  void RenderGround(const Array<Point>& visible_cells, const camera::Camera& cam) const;

  /// @brief 地面の境界を描画する。
  /// @param visible_cells 描画範囲内のノードの位置の配列
  /// @param cam カメラ。
  void RenderGroundEdges(const Array<Point>& visible_cells, const camera::Camera& cam) const;

  /// @brief 地面の境界を描画する際の色を計算する。
  /// @param pos 境界の位置。
  /// @return 描画する色。
  ColorF CalcEdgeColor(const Vec2& pos) const;

private:
  /// @brief 描画範囲内のセルの位置の配列を作成する。
  /// @param cam カメラ。
  /// @return 描画範囲内のセルの位置の配列
  Array<Point> CreateVisibleCells(const camera::Camera& cam) const;

private:
  NodeGrid node_grid_;
  MarchingSquares marching_squares_;

  Array<Point> sinhalite_positions_;

  HashTable<MaterialEnum, std::unique_ptr<IMaterial>> material_table_;

};

}
