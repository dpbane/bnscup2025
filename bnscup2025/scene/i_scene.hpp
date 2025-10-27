#pragma once

#include "common_data.hpp"

namespace bnscup2025::scene {

enum class SceneEnum {
  Title,
  Game,
  GameOver,
  Ending,
};

using App = SceneManager<SceneEnum, CommonData>;

}
