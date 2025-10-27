#pragma once

#include "i_scene.hpp"

#include "terrain/terrain.hpp"
#include "terrain/visibility.hpp"
#include "camera/camera.hpp"

#include "player/player.hpp"
#include "enemy/enemy.hpp"

namespace bnscup2025::scene {

class Game : public App::Scene {
public:
  Game(const InitData& init_data);

  /// @brief ゲームの状態を更新する。
  void update() override;

  /// @brief ゲームの状態を描画する。
  void draw() const override;

private:
  Optional<terrain::Terrain> terrain_;
  Optional<terrain::Visibility> visibility_;
  Optional<camera::Camera> camera_;

  Effect effect_;

  Optional<player::Player> player_;
  Optional<enemy::Enemy> enemy_;

  MSRenderTexture visibility_mask_texture_;
  Array<Triangle> visibility_triangles_;

};

}
