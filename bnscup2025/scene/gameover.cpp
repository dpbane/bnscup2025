#include "stdafx.h"
#include "gameover.hpp"

namespace bnscup2025::scene {

GameOver::GameOver(const InitData& init_data) :
  IScene(init_data) {
}

void GameOver::update() {
  timer_ += Scene::DeltaTime();

  if (timer_ >= 4.0) {
    getData() = CommonData {
      .next_level = 0,
      .next_room = Room::Shop,
      .power_grade = {},
      .death_count = getData().death_count + 1,
      .clear_count = getData().clear_count
    };
    changeScene(SceneEnum::Game, 0);
  }
}

void GameOver::draw() const {

  double alpha = 0.0;
  if (timer_ >= 1.0) {
    if (timer_ < 1.5) {
      alpha = (timer_ - 1.0) / 0.5;
    }
    else if (timer_ < 2.5) {
      alpha = 1.0;
    }
    else if (timer_ < 3.0) {
      alpha = 1.0 - (timer_ - 2.5) / 0.5;
    }
  }

  Scene::Rect().draw(ColorF(0.0));
  FontAsset(U"Tutorial")(U"シンでしまった…").drawAt(
    Scene::Center().movedBy(0, -Scene::Height() / 6),
    ColorF(1.0).withAlpha(alpha)
  );
}

}
