#include "stdafx.h"
#include "input.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::input {

bool Input::GamepadConnected() const {
  return XInput(0).isConnected();
}

void Input::Update() {
  if (GamepadConnected()) {
    input_data_ = GetGamepadData();
  }
  else {
    input_data_ = GetKeyboardAndMouseData();
  }
}

const InputData& Input::GetData() const {
  return input_data_;
}

InputData Input::GetKeyboardAndMouseData() const {
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

  ret.dig = MouseL.pressed();
  ret.action = MouseR.pressed();
  ret.confirm_trigger = (MouseL.down() || MouseR.down());
  ret.confirm_keep = (MouseL.pressed() || MouseR.pressed());
  ret.action_change_upright = Mouse::Wheel() < 0;
  ret.action_change_downleft = Mouse::Wheel() > 0;

  if (not screen::Fade::GetInstance().CompletedFadeIn()) {
    const Vec2 df = ret.direction_face;
    ret = InputData {};
    ret.direction_face = df;
  }

  return ret;
}

InputData Input::GetGamepadData() const {
  s3d::detail::XInput_impl gamepad = XInput(0);

  gamepad.setLeftTriggerDeadZone();
  gamepad.setRightTriggerDeadZone();
  gamepad.setLeftThumbDeadZone();
  gamepad.setRightThumbDeadZone();

  InputData ret {};
  Vec2 left_stick { gamepad.leftThumbX, -gamepad.leftThumbY };
  Vec2 right_stick { gamepad.rightThumbX, -gamepad.rightThumbY };
  if (left_stick.lengthSq() > 0.04) ret.direction_move = left_stick.normalized();
  if (right_stick.lengthSq() > 0.04) ret.direction_face = right_stick.normalized();

  ret.dig = gamepad.buttonRB.pressed();
  ret.action = gamepad.buttonLB.pressed();
  ret.confirm_trigger = ((not input_data_.dig && not input_data_.action) && (ret.dig || ret.action));
  ret.confirm_keep = (ret.dig || ret.action);
  ret.action_change_upright = not input_data_.action_change_upright_keep && gamepad.rightTrigger > 0.2;
  ret.action_change_downleft = not input_data_.action_change_downleft_keep && gamepad.leftTrigger > 0.2;
  ret.action_change_upright_keep = gamepad.rightTrigger > 0.2;
  ret.action_change_downleft_keep = gamepad.leftTrigger > 0.2;

  return ret;
}

}
