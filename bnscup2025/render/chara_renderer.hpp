#pragma once

#include "camera/camera.hpp"


namespace bnscup2025::render {

class CharaRenderer {
  CharaRenderer() = delete;
  CharaRenderer(const CharaRenderer&) = delete;
  CharaRenderer& operator=(const CharaRenderer&) = delete;

public:
  static void Render(
    const camera::Camera& camera,
    const Vec2& position,
    const Vec2& direction,
    const ColorF& color_body,
    const ColorF& color_edge,
    double bloom_x1,
    double bloom_x4,
    double bloom_x8
  );
};

}
