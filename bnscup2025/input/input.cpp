#include "stdafx.h"
#include "input.hpp"

#include "screen/fade.hpp"

namespace bnscup2025::input {

bool Input::GamepadConnected() const {
  return XInput(0).isConnected();
}

void Input::Update() {
  InputData ret {};

  // ゲームパッド
  s3d::detail::XInput_impl gamepad = XInput(0);
  gamepad.setLeftTriggerDeadZone();
  gamepad.setRightTriggerDeadZone();
  gamepad.setLeftThumbDeadZone();
  gamepad.setRightThumbDeadZone();
  Vec2 left_stick { gamepad.leftThumbX, -gamepad.leftThumbY };
  Vec2 right_stick { gamepad.rightThumbX, -gamepad.rightThumbY };

  // マウス
  Vec2 center = Scene::CenterF();
  Vec2 mouse = Cursor::PosF();

  // 向き入力
  if (Cursor::PreviousPosF() != mouse) {
    // マウスが動いていたらマウス優先
    ret.direction_face = (mouse - center).normalized();
    mouse_visible_ = true;
  }
  else {
    // 動いていなかったらゲームパッド優先
    if (right_stick.lengthSq() > 0.04) {
      ret.direction_face = right_stick.normalized();
      mouse_visible_ = false;
    }
  }
  if (not mouse_visible_) Cursor::RequestStyle(CursorStyle::Hidden);

  // 移動入力
  ret.direction_move = Vec2 { 0.0, 0.0 };
  if (KeyW.pressed()) ret.direction_move.y -= 1.0;
  if (KeyS.pressed()) ret.direction_move.y += 1.0;
  if (KeyA.pressed()) ret.direction_move.x -= 1.0;
  if (KeyD.pressed()) ret.direction_move.x += 1.0;
  if (not ret.direction_move.isZero()) ret.direction_move = ret.direction_move.normalized();
  if (ret.direction_move.isZero()) {
    // キーボード入力がなければゲームパッド優先
    if (left_stick.lengthSq() > 0.04) ret.direction_move = left_stick.normalized();
  }

  ret.dig = (MouseL | gamepad.buttonRB).pressed();
  ret.action = (MouseR | gamepad.buttonLB).pressed();
  ret.confirm_trigger = (MouseL | MouseR | gamepad.buttonRB | gamepad.buttonLB).down();
  ret.confirm_keep = (MouseL | MouseR | gamepad.buttonRB | gamepad.buttonLB).pressed();

  ret.action_change_upright = (Mouse::Wheel() < 0) || (not input_data_.action_change_upright_keep && gamepad.rightTrigger > 0.2);
  ret.action_change_downleft = (Mouse::Wheel() > 0) || (not input_data_.action_change_downleft_keep && gamepad.leftTrigger > 0.2);
  ret.action_change_upright_keep = gamepad.rightTrigger > 0.2;
  ret.action_change_downleft_keep = gamepad.leftTrigger > 0.2;

  if (not screen::Fade::GetInstance().CompletedFadeIn()) {
    const Vec2 df = ret.direction_face;
    ret = InputData {};
    ret.direction_face = df;
  }

  input_data_ = ret;
}

const InputData& Input::GetData() const {
  return input_data_;
}

}
