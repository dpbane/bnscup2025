#pragma once

namespace bnscup2025 {

class DebugVar {
  DebugVar() = default;
  DebugVar(const DebugVar&) = delete;
  DebugVar& operator=(const DebugVar&) = delete;

public:
  static DebugVar& GetInstance() {
    static DebugVar instance;
    return instance;
  };

public:
  bool invincible_mode_ { false };
  bool disable_visibility_mask_ { false };
  bool full_goshin_on_ { false };
  bool visible_enemy_cost_map_ { false };
};

}
