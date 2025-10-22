#pragma once

namespace bnscup2025::render {

class BlendMode {
private:
  BlendMode() = delete;
  BlendMode(const BlendMode&) = delete;
  BlendMode& operator=(const BlendMode&) = delete;

public:
  static ScopedRenderStates2D AlphaMax();

};

}
