#pragma once

#include "map_grid.hpp"
#include "terrain.hpp"

namespace bnscup2025::terrain {

class MapGenerator {
public:
  struct Parameters {
    Terrain terrain;
    Vec2 player_position;
    Vec2 enemy_position;
  };

private:
  MapGenerator() = delete;
  MapGenerator(const MapGenerator&) = delete;
  MapGenerator& operator=(const MapGenerator&) = delete;

public:
  static Parameters Generate(const Size& size, uint64 seed, int octaves, double persistence, double scale);


};

}
