#include "stdafx.h"
#include "input.hpp"

namespace bnscup2025::input {

InputData Input::GetData() {
  InputData ret {};

  Vec2 center = Scene::CenterF();
  Vec2 mouse = Cursor::PosF();
  ret.direction_face = (mouse - center).normalized();
  if (ret.direction_face.isZero()) ret.direction_face = Vec2(0.0, -1.0);

  ret.direction_move = Vec2 { 0.0, 0.0 };
  if (KeyW.pressed()) ret.direction_move.y -= 1.0;
  if (KeyS.pressed()) ret.direction_move.y += 1.0;
  if (KeyA.pressed()) ret.direction_move.x -= 1.0;
  if (KeyD.pressed()) ret.direction_move.x += 1.0;
  if (not ret.direction_move.isZero()) ret.direction_move = ret.direction_move.normalized();

  ret.dig = MouseL.pressed();
  ret.action = MouseR.pressed();

  return ret;
}

}
