#include "stdafx.h"
#include "game.hpp"

#include "terrain/visibility.hpp"

namespace bnscup2025::game {

Game::Game() :
  terrain_(terrain::NodeGrid(100, 100)) {
}

void Game::Update() {

  double speed = 5.0;
  if (KeyA.pressed()) {
    center_.x -= speed * Scene::DeltaTime();
  }
  if (KeyD.pressed()) {
    center_.x += speed * Scene::DeltaTime();
  }
  if (KeyW.pressed()) {
    center_.y -= speed * Scene::DeltaTime();
  }
  if (KeyS.pressed()) {
    center_.y += speed * Scene::DeltaTime();
  }
  if (KeyZ.pressed()) {
    scale_ *= Pow(1.5, Scene::DeltaTime());
  }
  if (KeyX.pressed()) {
    scale_ /= Pow(1.5, Scene::DeltaTime());
  }

  Size node_size = terrain_.GetNodeGridSize();

  for (int k = 0; k < 20; ++k) {
    int x = Random(1, node_size.x - 2);
    int y = Random(1, node_size.y - 2);
    terrain_.ModifyNode(Point { x, y }, Random(0.0, 1.0));
  }

  terrain_.Update();


}

void Game::Render() {

  terrain_.Render(center_, scale_);

  Vec2 offset = -(center_ - Scene::Size() / (2.0 * scale_)) * scale_;
  const Transformer2D t1 { Mat3x2::Translate(offset) };
  const Transformer2D t2 { Mat3x2::Scale(scale_) };

  const auto lines = terrain_.CreateVisibleWallLines(center_, scale_);
  Print << lines.size();

  terrain::Visibility visibility(terrain_);
  Vec2 p1 = Cursor::PosF();
  Vec2 p2 = Scene::CenterF();
  const auto visible = visibility.CalcVisibilityTriangles(center_, scale_, center_, (p1 - p2).normalized(), 100.0_deg, 100);
  Print << visible.size();


  for (const auto& tri : visible) {
    tri.draw(Palette::Yellow.withAlpha(128));
  }
}

}
