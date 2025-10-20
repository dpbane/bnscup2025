#pragma once

// https://qiita.com/m4saka/items/5da6cd4b57bc894d35dd

class FrameRateLimitAddon : public IAddon {
private:
  static constexpr std::chrono::steady_clock::duration MaxDrift = 10ms;
  std::chrono::steady_clock::duration m_oneFrameDuration;
  std::chrono::time_point<std::chrono::steady_clock> m_sleepUntil;

  // 1フレームあたりの時間を計算
  static std::chrono::steady_clock::duration FPSToOneFrameDuration(int32 targetFPS) {
    if (targetFPS <= 0) {
      throw Error { U"不正なフレームレート: {}"_fmt(targetFPS) };
    }
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(1s) / targetFPS;
  }

public:
  explicit FrameRateLimitAddon(int32 targetFPS)
    : m_oneFrameDuration(FPSToOneFrameDuration(targetFPS))
    , m_sleepUntil(std::chrono::steady_clock::now()) {
  }

  // 毎フレームの描画反映後に呼ばれる
  virtual void postPresent() override {
    m_sleepUntil += m_oneFrameDuration;
    m_sleepUntil = Max(m_sleepUntil, std::chrono::steady_clock::now() - MaxDrift);

    std::this_thread::sleep_until(m_sleepUntil);
  }

  // フレームレートを変更
  void setTargetFPS(int32 targetFPS) {
    m_oneFrameDuration = FPSToOneFrameDuration(targetFPS);
  }
};
