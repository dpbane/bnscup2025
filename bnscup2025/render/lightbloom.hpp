#pragma once

namespace bnscup2025::render {

class LightBloom {
private:
  LightBloom();
  LightBloom(const LightBloom&) = delete;
  LightBloom& operator=(const LightBloom&) = delete;

public:
  static const LightBloom& GetInstance() {
    static LightBloom instance;
    return instance;
  }

  ScopedRenderTarget2D CreateRenderTarget() const;

  void Apply(double x1, double x4, double x8) const;

private:
  MSRenderTexture blur_x1_texture_;
  RenderTexture blur_x1_internal_texture_;
  RenderTexture blur_x4_texture_;
  RenderTexture blur_x4_internal_texture_;
  RenderTexture blur_x8_texture_;
  RenderTexture blur_x8_internal_texture_;
};

}
