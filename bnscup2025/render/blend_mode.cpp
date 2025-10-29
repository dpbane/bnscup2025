#include "stdafx.h"
#include "blend_mode.hpp"

namespace bnscup2025::render {

ScopedRenderStates2D BlendMode::AlphaMax() {
  BlendState blendState = BlendState::Default2D;
  blendState.srcAlpha = Blend::SrcAlpha;
  blendState.dstAlpha = Blend::DestAlpha;
  blendState.opAlpha = BlendOp::Max;
  return ScopedRenderStates2D { blendState };
}

ScopedRenderStates2D BlendMode::AlphaMin() {
  BlendState blendState = BlendState::Default2D;
  blendState.srcAlpha = Blend::SrcAlpha;
  blendState.dstAlpha = Blend::DestAlpha;
  blendState.opAlpha = BlendOp::Min;
  return ScopedRenderStates2D { blendState };
}

}
