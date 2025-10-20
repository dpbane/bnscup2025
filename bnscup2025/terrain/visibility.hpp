#pragma once

#include "terrain.hpp"

namespace bnscup2025::terrain {

class Visibility {
public:
  /// @brief コンストラクタ。
  /// @param marching_squares マーチングスクエア法クラスの参照。
  Visibility(const Terrain& terrain);

  /// @brief 視点位置から見える範囲を計算する。
  /// @param center 画面の中心となるセル座標。(0, 0)が左上のセルの左上角に対応する。
  /// @param cell_size セル1つが何ピクセルになるか。
  /// @param eye_position 視点位置。
  /// @param direction 視点の向き。
  /// @param fov 視野角（ラジアン）。
  /// @param length 視距離。
  /// @return 見える範囲。
  Array<Triangle> CalcVisibilityTriangles(const Vec2& center, const SizeF& cell_size, const Vec2& eye_position, const Vec2& direction, double fov, double length) const;

private:
  const Terrain terrain_;

};

}
