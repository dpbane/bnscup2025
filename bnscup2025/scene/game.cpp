#include "stdafx.h"
#include "game.hpp"

#include "screen/fade.hpp"

#include "render/blend_mode.hpp"

#include "terrain/visibility.hpp"
#include "terrain/map_generator.hpp"

#include "input/input.hpp"
#include "npc/speaker_lines.hpp"

#include "npc/text_window.hpp"

namespace bnscup2025::scene {

Game::Game(const InitData& init_data) :
  IScene(init_data) {

  const int level = getData().next_level;
  is_game_ = (getData().next_room == Room::Game);

  auto map_params = terrain::MapGenerator::Generate(level, is_game_);
  terrain_.emplace(std::move(map_params.terrain));
  visibility_.emplace(*terrain_);

  const double shorter_side = Min(Scene::Width(), Scene::Height());
  const double cell_size = shorter_side / 54.0;
  camera_.emplace(Vec2 { 0.0, 0.0 }, SizeF { cell_size, cell_size });

  enemy::EnemyParameters enemy_params {
    .sound_hear_radius = 10.0,
    .view_radius = 20.0,
    .prowl_speed = 4.0,
    .to_sound_speed = 4.5,
    .pursuit_speed = 5.0
  };

  player_.emplace(*camera_, *terrain_, effect_, map_params.player_position, is_game_);
  exit_.emplace(*camera_, *player_, map_params.exit_position);

  if (is_game_) {
    if (level == 0) {
      // チュートリアル
      tutorial_text_.emplace(*player_);
      tutorial_text_->AddEntry(
        npc::TutorialText::Entry {
          .area = RectF { 0.0, 45.0, 20.0, 15.0 },
          .text = U"WASDキー / 左スティックで移動"
        }
      );
      tutorial_text_->AddEntry(
        npc::TutorialText::Entry {
          .area = RectF { 0.0, 30.0, 20.0, 10.0 },
          .text = U"マウスカーソル / 右スティックで周囲を見渡す"
        }
      );
      tutorial_text_->AddEntry(
        npc::TutorialText::Entry {
          .area = RectF { 0.0, 15.0, 20.0, 10.0 },
          .text = U"左クリック / LTで地形を掘る"
        }
      );
      tutorial_text_->AddEntry(
        npc::TutorialText::Entry {
          .area = RectF { 0.0, 0.0, 20.0, 10.0 },
          .text = U"出口に乗り続けると次のエリアへ"
        }
      );
    }
    else {
      // 通常レベル
      enemy_.emplace(*camera_, *terrain_, *player_, map_params.enemy_position, enemy_params);
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

    // TODO: shop_.emplace()
  }

  npc::TextWindow::GetInstance().Reset();

  screen::Fade::GetInstance().BeginFadeIn(kFadeDuration);

}

void Game::update() {
  auto& fade = screen::Fade::GetInstance();
  const auto input_data = input::Input::GetInstance().GetData();

  player_->Update();
  if (fade.CompletedFadeIn()) {
    if (exit_) exit_->Update();

    if (enemy_) enemy_->Update();
    if (speaker_) speaker_->Update();

    if (tutorial_text_) tutorial_text_->Update();
  }

  if (enemy_ && enemy_->IsPlayerCaught()) {
    changeScene(SceneEnum::GameOver, 0);
  };

  camera_->SetCenter(player_->GetPosition());
  terrain_->Update();
  visibility_mask_.SetTriangles(
    visibility_->CalcVisibilityTriangles(*camera_, player_->GetShiftedPosition(), player_->GetDirectionFace().normalized(), 120.0_deg, 100)
  );
  visibility_mask_.SetPosition(player_->GetShiftedPosition());

  if (exit_ && exit_->ShouldExitGame() && fade.CompletedFadeIn()) {
    if (getData().next_room == Room::Shop) {
      getData() = CommonData {
        .next_level = getData().next_level + 1,
        .next_room = Room::Game,
        .power_grade = getData().power_grade,
        .death_count = getData().death_count,
        .clear_count = getData().clear_count,
      };
    }
    else {
      getData() = CommonData {
        .next_level = getData().next_level,
        .next_room = Room::Shop,
        .power_grade = getData().power_grade,
        .death_count = getData().death_count,
        .clear_count = getData().clear_count
      };
    }
    fade.BeginFadeOut(kFadeDuration);
  }
  if (fade.CompletedFadeOut()) {
    changeScene(SceneEnum::Game, 0);
  }

  npc::TextWindow::GetInstance().Update();

  fade.Update();
}

void Game::draw() const {

  terrain_->Render(*camera_);
  if (exit_) exit_->Render();

  player_->Render();
  if (enemy_) enemy_->Render();
  if (speaker_) speaker_->Render();

  const auto lines = terrain_->CreateVisibleWallLines(*camera_);
  //Print << U"スクリーン上に描画されうる線分の数：" << lines.size();
  //Print << U"可視範囲の三角形の数：" << visibility_triangles_.size();

  effect_.update();

  // 視界制限
  if (is_game_) {
    const double radius = 24.0;
    visibility_mask_.Render(*camera_, radius);
  }

  if (tutorial_text_) tutorial_text_->Render();

  npc::TextWindow::GetInstance().Render();
  screen::Fade::GetInstance().Render();
}

}
