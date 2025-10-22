#include "stdafx.h"
#include "camera.hpp"

namespace bnscup2025::camera {

Transformer2D Camera::CreateRenderTransformer() const {
  Vec2 offset = -(center_ - Scene::Size() / (2.0 * cell_size_)) * cell_size_;
  Transformer2D t { Mat3x2::Scale(cell_size_).translated(offset) };
  return t;
}

}
