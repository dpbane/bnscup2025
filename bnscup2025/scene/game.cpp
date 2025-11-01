#include "stdafx.h"
#include "game.hpp"

#include "debug_var.hpp"
#include "screen/fade.hpp"

#include "render/blend_mode.hpp"

#include "terrain/visibility.hpp"
#include "terrain/map_generator.hpp"

#include "input/input.hpp"
#include "npc/speaker_lines.hpp"

#include "ui/text_window.hpp"
#include "floor_name_factory.hpp"
#include "enemy/enemy_level_designer.hpp"

namespace bnscup2025::scene {

Game::Game(const InitData& init_data) :
  IScene(init_data) {

  if (not render_texture_) render_texture_.emplace(Scene::Size());

  const int level = getData().next_level;
  is_game_ = (getData().next_room == Room::Game);

  auto map_params = terrain::MapGenerator::Generate(level, is_game_);
  terrain_.emplace(std::move(map_params.terrain));
  visibility_.emplace(*terrain_);

  const double shorter_side = Min(Scene::Width(), Scene::Height());
  const double cell_size = shorter_side / player::GradeValueConverter { getData().power_grade }.GetViewDistance();
  camera_.emplace(Vec2 { 0.0, 0.0 }, SizeF { cell_size, cell_size });

  player_.emplace(*camera_, *terrain_, effect_, map_params.player_position, is_game_, getData().power_grade, getData().initial_goshin_item);
  exit_.emplace(*camera_, *player_, map_params.exit_position);

  if (is_game_) {
    if (level == 0) {
      // チュートリアル
      tutorial_text_.emplace(*player_);
      tutorial_text_->AddEntry(
        ui::TutorialText::Entry {
          .area = RectF { 0.0, 55.0, 20.0, 15.0 },
          .text = U"WASDキー / 左スティックで移動。"
        }
      );
      tutorial_text_->AddEntry(
        ui::TutorialText::Entry {
          .area = RectF { 0.0, 40.0, 20.0, 10.0 },
          .text = U"マウスカーソル / 右スティックで周囲を見渡す。"
        }
      );
      tutorial_text_->AddEntry(
        ui::TutorialText::Entry {
          .area = RectF { 0.0, 20.0, 20.0, 15.0 },
          .text = U"左クリック / RBで地形を掘る。長押し可。色が暗い壁は掘れない。"
        }
      );
      tutorial_text_->AddEntry(
        ui::TutorialText::Entry {
          .area = RectF { 0.0, 0.0, 20.0, 15.0 },
          .text = U"出口に乗り続けると次のエリアへ。"
        }
      );
    }
    else {
      // 通常レベル
      enemy_.emplace(*camera_, *terrain_, *player_, map_params.enemy_position, enemy::EnemyLevelDesigner::MakeParameters(level));
    }
  }
  else {
    // 休憩エリア
    speaker_.emplace(
      *camera_,
      *player_,
      map_params.speaker_position,
      npc::SpeakerLines::Get(level, getData().death_count, getData().clear_count),
      (level == 0 || level >= 12) ? npc::SpeakerEnum::Orrange : npc::SpeakerEnum::Sky
    );
    if (level >= 1 && level <= 11) {
      shop_.emplace(*camera_, *player_, getData().power_grade, map_params.shop_position, getData().sinhalite_amount);
    }
  }

  ui::TextWindow::GetInstance().Reset();

  screen::Fade::GetInstance().BeginFadeIn(kFadeDuration);

  floor_text_.emplace(FloorNameFactory::Create(level, is_game_));
  sinhalite_text_.emplace(getData().sinhalite_amount);
}

void Game::update() {
  auto& fade = screen::Fade::GetInstance();

  player_->Update();
  if (fade.CompletedFadeIn()) {
    if (exit_) exit_->Update();

    if (enemy_) enemy_->Update();
    if (speaker_) speaker_->Update();
    if (shop_) shop_->Update();

    if (tutorial_text_) tutorial_text_->Update();
  }

  if (enemy_ && enemy_->IsPlayerCaught()) {
    changeScene(SceneEnum::GameOver, 0);
  };

  camera_->SetCenter(player_->GetPosition());

  const double shorter_side = Min(Scene::Width(), Scene::Height());
  const double cell_size = shorter_side / player::GradeValueConverter { getData().power_grade }.GetViewDistance();
  camera_->SetCellSize(SizeF(cell_size, cell_size));

  getData().sinhalite_amount += terrain_->GetEarnedSinhalite();
  terrain_->Update();


  const double fov = player::GradeValueConverter { getData().power_grade }.GetViewAngle();
  const double distance = player::GradeValueConverter { getData().power_grade }.GetViewDistance();
  visibility_mask_.SetTriangles(
    visibility_->CalcVisibilityTriangles(*camera_, player_->GetShiftedPosition(), player_->GetDirectionFace().normalized(), fov, distance)
  );
  visibility_mask_.SetPosition(player_->GetShiftedPosition());


  if (exit_ && exit_->ShouldExitGame() && fade.CompletedFadeIn()) {
    if (getData().next_room == Room::Shop) {
      getData().next_level += 1;
      getData().next_room = Room::Game;
    }
    else {
      getData().next_room = Room::Shop;
      if (getData().next_level == 12) {
        if (getData().sinhalite_amount >= 60) {
          getData().next_level = 13;
        }
        if (getData().power_grade[player::PowerGradeItem::God] >= 1) {
          getData().next_level = 14;
        }
      }
    }
    fade.BeginFadeOut(kFadeDuration);
  }
  if (fade.CompletedFadeOut()) {
    getData().initial_goshin_item = player_->GetSelectedGoshinItem();
    if (getData().next_room == Room::End) {
      changeScene(SceneEnum::Ending, 0);
    }
    else {
      changeScene(SceneEnum::Game, 0);
    }
  }

  if (getData().next_level >= 12 && getData().next_room == Room::Shop) {
    if (speaker_ && speaker_->IsFinished() && not fade.IsFadingOut()) {
      fade.BeginFadeOut(kFadeDuration);
      getData().next_room = Room::End;
    }
  }

  ui::TextWindow::GetInstance().Update();
  floor_text_->Update();
  sinhalite_text_->Update();

  fade.Update();

}

void Game::draw() const {

  {
    const ScopedRenderTarget2D target { render_texture_->clear(ColorF{0.0}) };

    terrain_->Render(*camera_);
    if (exit_) exit_->Render();

    player_->Render();
    if (enemy_) enemy_->Render();
    if (speaker_) speaker_->Render();
    if (shop_) shop_->Render();

    const auto lines = terrain_->CreateVisibleWallLines(*camera_);
    //Print << U"スクリーン上に描画されうる線分の数：" << lines.size();
    //Print << U"可視範囲の三角形の数：" << visibility_triangles_.size();

    effect_.update();
  }
  Graphics2D::Flush();
  render_texture_->resolve();
  render_texture_->draw();

  {
    const ScopedRenderTarget2D target { render_texture_->clear(ColorF{0.0}) };
    render_texture_->draw(ColorF { 0.1, 1.0, 0.2 });
  }
  Graphics2D::Flush();
  render_texture_->resolve();

  // 視界制限
  const double view_distance = player::GradeValueConverter { getData().power_grade }.GetViewDistance();
  const double visible_radius = 24.0 * view_distance / 54.0;
  if (is_game_ && getData().power_grade[player::PowerGradeItem::God] == 0 && not DebugVar::GetInstance().disable_visibility_mask_) {
    visibility_mask_.Render(*camera_, visible_radius);
  }

  // 心
  if (player_->GetKokoroAlpha() > 0.0) {
    render_texture_->draw(ColorF { 1.0, player_->GetKokoroAlpha() });
  }

  // 視界制限（円）
  if (is_game_ && getData().power_grade[player::PowerGradeItem::God] == 0 && not DebugVar::GetInstance().disable_visibility_mask_) {
    visibility_mask_.RenderCircle(*camera_, visible_radius);
  }

  if (tutorial_text_) tutorial_text_->Render();

  ui::TextWindow::GetInstance().Render();
  floor_text_->Render();
  sinhalite_text_->Render();

  const double shorter_side = Min(Scene::Width(), Scene::Height());
  player_->RenderUI(visible_radius * shorter_side / view_distance);
  if (enemy_) enemy_->RenderUI();

  screen::Fade::GetInstance().Render();

}

}
