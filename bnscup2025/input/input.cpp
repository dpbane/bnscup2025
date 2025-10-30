#include "stdafx.h"
#include "input.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::input {

InputData Input::GetData() {
  InputData ret {};

  Vec2 center = Scene::CenterF();
  Vec2 mouse = Cursor::PosF();
  ret.direction_face = (mouse - center).normalized();
  if (ret.direction_face.isZero()) ret.direction_face = Vec2(0.0, 0.0);

  ret.direction_move = Vec2 { 0.0, 0.0 };
  if (KeyW.pressed()) ret.direction_move.y -= 1.0;
  if (KeyS.pressed()) ret.direction_move.y += 1.0;
  if (KeyA.pressed()) ret.direction_move.x -= 1.0;
  if (KeyD.pressed()) ret.direction_move.x += 1.0;
  if (not ret.direction_move.isZero()) ret.direction_move = ret.direction_move.normalized();

  ret.shift = KeyShift.pressed();
  ret.dig = MouseL.pressed();
  ret.action = MouseR.pressed();
  ret.confirm_trigger = (MouseL.down() || MouseR.down());
  ret.confirm_keep = (MouseL.pressed() || MouseR.pressed());

  if (not screen::Fade::GetInstance().CompletedFadeIn()) {
    const Vec2 df = ret.direction_face;
    ret = InputData {};
    ret.direction_face = df;
  }

  return ret;
}

}
