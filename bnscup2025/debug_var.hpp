#pragma once

namespace bnscup2025 {

class DebugVar {
  DebugVar();
  DebugVar(const DebugVar&) = delete;
  DebugVar& operator=(const DebugVar&) = delete;

public:
  static DebugVar& GetInstance() {
    static DebugVar instance;
    return instance;
  }

private:


};

}
