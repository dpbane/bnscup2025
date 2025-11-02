#include "material.hpp"

namespace bnscup2025::terrain {

constexpr MaterialEnum M = MaterialEnum::Bounds;

template<> ColorF Material<M>::GetBaseColor(int level) const { return ColorF { -0.1, -0.1, -0.1 }; }
template<> double Material<M>::GetSinhaliteEffectiveness() const { return 0.0; }
template<> double Material<M>::GetDiggability() const { return 0.0; }

}
