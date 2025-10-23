#pragma once

namespace bnscup2025::input {

struct InputData {
  Vec2 direction_face;
  Vec2 direction_move;
  bool dig;
  bool action;
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

  InputData GetData();

};


}
