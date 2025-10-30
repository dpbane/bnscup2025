#pragma once

#include "player/power_grade.hpp"

namespace bnscup2025::npc {

struct ItemInfo {
  String name;
  String description;
  Array<int> costs;
};

class ItemInfoFactory {
  ItemInfoFactory() = delete;
  ItemInfoFactory(const ItemInfoFactory&) = delete;
  ItemInfoFactory& operator=(const ItemInfoFactory&) = delete;

public:
  static ItemInfo Get(player::PowerGradeItem item);

};

}
