#include "stdafx.h"
#include "visibility_mask.hpp"

#include "render/blend_mode.hpp"

namespace bnscup2025::render {

VisibilityMask::VisibilityMask() :
  visibility_mask_texture_(Scene::Size()),
  sub_mask_texture_(Scene::Size()) {
}

void VisibilityMask::SetTriangles(Array<Triangle>&& triangles) {
  visibility_triangles_ = std::move(triangles);
}

void VisibilityMask::SetPosition(const Vec2& position) {
  position_ = position;
}

void VisibilityMask::Render(const camera::Camera& camera, double radius) const {
  {
    // マスクを生成
    const ScopedRenderTarget2D target { visibility_mask_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 1.0 }) };

    // 可視範囲を描画
    {
      const auto t = camera.CreateRenderTransformer();
      const auto blend = render::BlendMode::AlphaMin();
      for (const auto& tri : visibility_triangles_) {
        tri.draw(ColorF { 1.0, 0.0 });
      }
      Circle { position_, 2.5 }.draw(ColorF { 1.0, 0.0 });
    }
  }

  // 不可視範囲を描画
  {
    const ScopedRenderTarget2D target { sub_mask_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 1.0 }) };
    const auto t = camera.CreateRenderTransformer();
    const auto blend = render::BlendMode::AlphaMin();
    Circle { position_, radius }.draw(ColorF { 0.0, 0.0 });
  }

  Graphics2D::Flush();
  visibility_mask_texture_.resolve();
  sub_mask_texture_.resolve();

  visibility_mask_texture_.draw();
  sub_mask_texture_.draw();
}



}
