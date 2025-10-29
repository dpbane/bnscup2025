#pragma once

#include "camera/camera.hpp"
#include "player/player.hpp"

namespace bnscup2025::exit {

/// @brief 出口オブジェクト
class Exit {
public:
  Exit(const camera::Camera& camera, const player::Player& player, const Vec2& position);

  /// @brief 状態を更新する。
  void Update();

  /// @brief 出口を描画する。
  void Render() const;

  /// @brief プレイヤーが出口に乗っているかを取得する。
  bool IsPlayerOnExit() const;

  /// @brief プレイヤーが出口に乗っている時間を取得する。
  double GetPlayerOnExitTime() const;

  /// @brief 次のシーンに移行すべきかを取得する。
  bool ShouldExitGame() const;

private:
  const camera::Camera& camera_;
  const player::Player& player_;
  const Vec2 position_;
  double timer_ { 0.0 };

};

}
