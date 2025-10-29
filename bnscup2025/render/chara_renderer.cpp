#include "stdafx.h"
#include "chara_renderer.hpp"

#include "lightbloom.hpp"
#include "blend_mode.hpp"

namespace bnscup2025::render {

void CharaRenderer::Render(
  const camera::Camera& camera,
  const Vec2& position,
  const Vec2& direction,
  const ColorF& color_body,
  const ColorF& color_edge,
  double bloom_x1,
  double bloom_x4,
  double bloom_x8) {

  const auto& lightbloom = render::LightBloom::GetInstance();

  // 本体
  {
    const auto target = lightbloom.CreateRenderTarget();
    const auto blend = render::BlendMode::AlphaMax();
    const auto t = camera.CreateRenderTransformer();

    // 体（円）
    Circle { position, kCharacterRadius }
      .draw(color_body)
      .drawFrame(0.20, color_edge);

    // 目
    const auto& v1 = direction;
    const auto v2 = direction.rotated90();
    const auto p1 = position + v1 * 0.25;
    const auto pl1 = p1 + v2 * 0.2;
    const auto pl2 = pl1 + v1 * 0.2;
    const auto pr1 = p1 - v2 * 0.2;
    const auto pr2 = pr1 + v1 * 0.2;
    Line { pl1, pl2 }.draw(LineStyle::RoundCap, 0.20, color_edge);
    Line { pr1, pr2 }.draw(LineStyle::RoundCap, 0.20, color_edge);
  }
  lightbloom.Apply(bloom_x1, bloom_x4, bloom_x8);
}

}
