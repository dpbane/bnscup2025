#pragma once

#include "map_grid.hpp"

namespace bnscup2025::terrain {

class MapGenerator {
  MapGenerator() = delete;
  MapGenerator(const MapGenerator&) = delete;
  MapGenerator& operator=(const MapGenerator&) = delete;

public:
  static NodeGrid Generate(const Size& size, int octaves, double persistence, double scale);


};

}
