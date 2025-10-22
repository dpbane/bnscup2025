#pragma once

namespace bnscup2025::camera {

class Camera {
public:
  Camera(const Vec2& center, const SizeF& cell_size) :
    center_(center), cell_size_(cell_size) {
  }

  /// @brief 描画する際の座標変換を作成する。
  /// @return 座標変換オブジェクト。
  Transformer2D CreateRenderTransformer() const;

  void SetCenter(const Vec2& center) { center_ = center; }
  void SetCellSize(const SizeF& cell_size) { cell_size_ = cell_size; }
  Vec2 GetCenter() const { return center_; }
  SizeF GetCellSize() const { return cell_size_; }

private:
  Vec2 center_;
  SizeF cell_size_;
};

}
