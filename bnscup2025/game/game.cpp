#include "stdafx.h"
#include "game.hpp"

#include "terrain/visibility.hpp"
#include "terrain/map_generator.hpp"

namespace bnscup2025::game {

Game::Game() :
  terrain_(terrain::MapGenerator::Generate(Size(100, 100), 2, 0.5, 10.0)),
  visibility_mask_texture_(Scene::Size()) {
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

  terrain_.Update();

}

void Game::Render() {

  terrain_.Render(center_, scale_);



  const auto lines = terrain_.CreateVisibleWallLines(center_, scale_);
  Print << U"スクリーン上に描画されうる線分の数：" << lines.size();

  terrain::Visibility visibility(terrain_);
  Vec2 p1 = Cursor::PosF();
  Vec2 p2 = Scene::CenterF();
  const auto visible = visibility.CalcVisibilityTriangles(center_, scale_, center_, (p1 - p2).normalized(), 100.0_deg, 100);
  Print << U"可視範囲の三角形の数：" << visible.size();

  {
    const ScopedRenderTarget2D target { visibility_mask_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 0.0 }) };
    BlendState blendState = BlendState::Default2D;
    blendState.srcAlpha = Blend::SrcAlpha;
    blendState.dstAlpha = Blend::DestAlpha;
    blendState.opAlpha = BlendOp::Max;
    const auto t = terrain_.CreateRenderTransformer(center_, scale_);
    for (const auto& tri : visible) {
      tri.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
    }
    Circle { center_, 1.0 }.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
  }
  Graphics2D::Flush();
  visibility_mask_texture_.resolve();
  {
    const ScopedRenderStates2D blend { BlendState::Multiplicative };
    visibility_mask_texture_.draw();
  }
}

}
