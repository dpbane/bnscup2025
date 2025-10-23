#pragma once

#include "terrain.hpp"
#include "camera/camera.hpp"

namespace bnscup2025::terrain {

class Visibility {
public:
  /// @brief コンストラクタ。
  /// @param marching_squares マーチングスクエア法クラスの参照。
  Visibility(const Terrain& terrain);

  /// @brief 視点位置から見える範囲を計算する。
  /// @param cam カメラ。
  /// @param eye_position 視点位置。
  /// @param direction 視点の向き。
  /// @param fov 視野角（ラジアン）。
  /// @param length 視距離。
  /// @return 見える範囲。
  Array<Triangle> CalcVisibilityTriangles(const camera::Camera& cam, const Vec2& eye_position, const Vec2& direction, double fov, double length) const;

private:
  const Terrain& terrain_;

};

}
