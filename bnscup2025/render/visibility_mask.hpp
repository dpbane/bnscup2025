#pragma once

#include "camera/camera.hpp"

namespace bnscup2025::render {

class VisibilityMask {
public:
  VisibilityMask();

  void SetTriangles(Array<Triangle>&& triangles);
  void SetPosition(const Vec2& position);
  void Render(const camera::Camera& camera, double radius) const;
  void RenderCircle(const camera::Camera& camera, double radius) const;

private:
  MSRenderTexture visibility_mask_texture_;
  Array<Triangle> visibility_triangles_;
  Vec2 position_ {};
};

}
