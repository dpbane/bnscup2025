#include "stdafx.h"
#include "lightbloom.hpp"

namespace bnscup2025::render {

LightBloom::LightBloom() :
  blur_x1_texture_(Scene::Size()),
  blur_x1_internal_texture_(Scene::Size()),
  blur_x4_texture_(Scene::Size() / 4),
  blur_x4_internal_texture_(Scene::Size() / 4),
  blur_x8_texture_(Scene::Size() / 8),
  blur_x8_internal_texture_(Scene::Size() / 8) {
}

ScopedRenderTarget2D LightBloom::CreateRenderTarget() const {
  return ScopedRenderTarget2D(blur_x1_texture_.clear(ColorF { 0, 0, 0, 0.0 }));
}

void LightBloom::Apply(double x1, double x4, double x8) const {
  // x1バッファの描画予約を解決
  Graphics2D::Flush();
  blur_x1_texture_.resolve();

  // まず描画
  blur_x1_texture_.draw();

  {
    const ScopedRenderStates2D blend { BlendState::Additive };
    // x1ブラー
    if (x1 > 0.0 || x4 > 0.0 || x8 > 0.0) {
      Shader::GaussianBlur(blur_x1_texture_, blur_x1_internal_texture_, blur_x1_texture_);
      blur_x1_texture_.resized(Scene::Size()).draw(ColorF { x1 });
    }
    // x4ブラー
    if (x4 > 0.0 || x8 > 0.0) {
      Shader::Downsample(blur_x1_texture_, blur_x4_texture_);
      Shader::GaussianBlur(blur_x4_texture_, blur_x4_internal_texture_, blur_x4_texture_);
      blur_x4_texture_.resized(Scene::Size()).draw(ColorF { x4 });
    }
    // x8ブラー
    if (x8 > 0.0) {
      Shader::Downsample(blur_x1_texture_, blur_x8_texture_);
      Shader::GaussianBlur(blur_x8_texture_, blur_x8_internal_texture_, blur_x8_texture_);
      blur_x8_texture_.resized(Scene::Size()).draw(ColorF { x8 });
    }
  }
}


}
