#include "stdafx.h"
#include "floor_name_factory.hpp"

namespace bnscup2025::scene {

String FloorNameFactory::Create(int level, bool is_game) {
  if (level == 0 && is_game) return U"";
  if (level == 0 && not is_game) return U"～ 洞窟の入り口 ～";
  if (level >= 12 && is_game) return U"～ 最深部 ～";

  if (not is_game) return U"";
  return U"～ 地下{:}階 ～"_fmt(level);
}

}
