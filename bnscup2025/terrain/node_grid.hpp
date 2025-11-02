#pragma once

#include "grid_points.hpp"
#include "material/material.hpp"

namespace bnscup2025::terrain {

struct NodeInfo {
  double density { 0.0 };
  MaterialEnum material;
};

using NodeGrid = GridPoints<NodeInfo>;

}
