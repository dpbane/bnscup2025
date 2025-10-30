#pragma once

namespace bnscup2025::scene {

class FloorNameFactory {
  FloorNameFactory() = delete;
  FloorNameFactory(const FloorNameFactory&) = delete;
  FloorNameFactory& operator=(const FloorNameFactory&) = delete;

public:
  static String Create(int level, bool is_game);

};

}
