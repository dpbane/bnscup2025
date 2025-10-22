#include "stdafx.h"
#include "game.hpp"

#include "terrain/visibility.hpp"
#include "terrain/map_generator.hpp"

namespace bnscup2025::game {

Game::Game() :
  terrain_(terrain::MapGenerator::Generate(Size(100, 100), RandomUint64(), 2, 0.5, 10.0)),
  camera_(Vec2 { 0.0, 0.0 }, SizeF { 50.0, 50.0 }),
  visibility_mask_texture_(Scene::Size()) {
}

void Game::Update() {

  double speed = 5.0;
  Vec2 center = camera_.GetCenter();
  SizeF scale = camera_.GetCellSize();

  if (KeyA.pressed()) {
    center.x -= speed * Scene::DeltaTime();
  }
  if (KeyD.pressed()) {
    center.x += speed * Scene::DeltaTime();
  }
  if (KeyW.pressed()) {
    center.y -= speed * Scene::DeltaTime();
  }
  if (KeyS.pressed()) {
    center.y += speed * Scene::DeltaTime();
  }
  if (KeyZ.pressed()) {
    scale *= Pow(1.5, Scene::DeltaTime());
  }
  if (KeyX.pressed()) {
    scale /= Pow(1.5, Scene::DeltaTime());
  }

  camera_.SetCenter(center);
  camera_.SetCellSize(scale);

  terrain_.Update();

}

void Game::Render() {

  terrain_.Render(camera_);



  const auto lines = terrain_.CreateVisibleWallLines(camera_);
  Print << U"スクリーン上に描画されうる線分の数：" << lines.size();

  terrain::Visibility visibility(terrain_);
  Vec2 p1 = Cursor::PosF();
  Vec2 p2 = Scene::CenterF();
  const auto visible = visibility.CalcVisibilityTriangles(camera_, camera_.GetCenter(), (p1 - p2).normalized(), 100.0_deg, 100);
  Print << U"可視範囲の三角形の数：" << visible.size();

  {
    const ScopedRenderTarget2D target { visibility_mask_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 0.0 }) };
    BlendState blendState = BlendState::Default2D;
    blendState.srcAlpha = Blend::SrcAlpha;
    blendState.dstAlpha = Blend::DestAlpha;
    blendState.opAlpha = BlendOp::Max;
    const auto t = camera_.CreateRenderTransformer();
    for (const auto& tri : visible) {
      tri.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
    }
    Circle { camera_.GetCenter(), 1.0 }.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
  }
  Graphics2D::Flush();
  visibility_mask_texture_.resolve();
  {
    const ScopedRenderStates2D blend { BlendState::Multiplicative };
    visibility_mask_texture_.draw();
  }
}

}
