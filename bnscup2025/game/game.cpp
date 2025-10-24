#include "stdafx.h"
#include "game.hpp"

#include "render/blend_mode.hpp"

#include "terrain/visibility.hpp"
#include "terrain/map_generator.hpp"

#include "input/input.hpp"

namespace bnscup2025::game {

Game::Game() :
  visibility_mask_texture_(Scene::Size()) {

  auto map_params = terrain::MapGenerator::Generate(Size(50, 50), RandomUint64(), 2, 0.5, 10.0);
  terrain_.emplace(std::move(map_params.terrain));
  visibility_.emplace(*terrain_);
  camera_.emplace(Vec2 { 0.0, 0.0 }, SizeF { 50.0, 50.0 });

  player_.emplace(*camera_, *terrain_, effect_, map_params.player_position.movedBy(0.5, 0.5));
  enemy_.emplace(*camera_, *terrain_, *player_, map_params.enemy_position.movedBy(0.5, 0.5));

}

void Game::Update() {

  const auto input_data = input::Input::GetInstance().GetData();

  player_->Update();
  enemy_->Update();

  camera_->SetCenter(player_->GetPosition());
  terrain_->Update();
  visibility_triangles_ = visibility_->CalcVisibilityTriangles(*camera_, player_->GetShiftedPosition(), player_->GetDirectionFace().normalized(), 120.0_deg, 100);

}

void Game::Render() {

  terrain_->Render(*camera_);

  player_->Render();
  enemy_->Render();


  const auto lines = terrain_->CreateVisibleWallLines(*camera_);
  //Print << U"スクリーン上に描画されうる線分の数：" << lines.size();
  //Print << U"可視範囲の三角形の数：" << visibility_triangles_.size();

  effect_.update();

  {
    const ScopedRenderTarget2D target { visibility_mask_texture_.clear(ColorF{ 0.0, 0.0, 0.0, 0.0 }) };
    const auto blend = render::BlendMode::AlphaMax();
    const auto t = camera_->CreateRenderTransformer();
    for (const auto& tri : visibility_triangles_) {
      tri.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
    }
    Circle { player_->GetShiftedPosition(), 1.0 }.draw(ColorF { 1.0, 1.0, 1.0, 1.0 });
  }
  Graphics2D::Flush();
  visibility_mask_texture_.resolve();
  {
    const ScopedRenderStates2D blend { BlendState::Multiplicative };
    visibility_mask_texture_.draw();
  }
}

}
