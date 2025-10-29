#pragma once

#include "i_scene.hpp"

#include "terrain/terrain.hpp"
#include "terrain/visibility.hpp"
#include "camera/camera.hpp"

#include "player/player.hpp"
#include "enemy/enemy.hpp"
#include "exit/exit.hpp"
#include "npc/speaker.hpp"

#include "render/visibility_mask.hpp"

namespace bnscup2025::scene {

class Game : public App::Scene {
public:
  Game(const InitData& init_data);

  /// @brief ゲームの状態を更新する。
  void update() override;

  /// @brief ゲームの状態を描画する。
  void draw() const override;

private:
  bool is_game_;

  Optional<terrain::Terrain> terrain_;
  Optional<terrain::Visibility> visibility_;
  Optional<camera::Camera> camera_;

  Effect effect_;

  Optional<player::Player> player_;
  Optional<enemy::Enemy> enemy_;
  Optional<exit::Exit> exit_;
  Optional<npc::Speaker> speaker_;

  render::VisibilityMask visibility_mask_;

};

}
