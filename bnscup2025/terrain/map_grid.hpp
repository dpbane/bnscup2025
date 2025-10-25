#pragma once

#include "grid_points.hpp"
#include "material/material.hpp"

namespace bnscup2025::terrain {

struct NodeInfo {
  double density;
  MaterialEnum material;
};

using NodeGrid = GridPoints<NodeInfo>;

}
