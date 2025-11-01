#pragma once

#include "marching_squares.hpp"

namespace bnscup2025::terrain {

class PushbackService {
  PushbackService() = delete;
  PushbackService(const PushbackService&) = delete;
  PushbackService& operator=(const PushbackService&) = delete;

public:
  static Vec2 Exec(const MarchingSquares& marching_squares, const Circle& circle);

private:
  static Vec2 DoOneStep(const MarchingSquares& marching_squares, const Circle& circle);

};

}
