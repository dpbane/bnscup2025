#pragma once

namespace bnscup2025::input {

struct InputData {
  Vec2 direction_face { 0, 0 };
  Vec2 direction_move { 0, 0 };
  bool dig : 1{ false };
  bool action : 1 { false };
  bool action_change_upright : 1 { false };
  bool action_change_downleft : 1 {false};
  bool action_change_upright_keep : 1 { false };
  bool action_change_downleft_keep : 1 { false };
  bool confirm_trigger : 1 { false };
  bool confirm_keep : 1 { false };
};

class Input {
  Input() = default;
  Input(const Input&) = delete;
  Input& operator=(const Input&) = delete;

public:
  static Input& GetInstance() {
    static Input instance;
    return instance;
  }

  bool GamepadConnected() const;

  void Update();
  const InputData& GetData() const;

private:
  InputData GetKeyboardAndMouseData() const;
  InputData GetGamepadData() const;

private:
  InputData input_data_;

};


}
