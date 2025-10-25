#include "material.hpp"

namespace bnscup2025::terrain {

constexpr MaterialEnum M = MaterialEnum::HardRock;

template<> ColorF Material<M>::GetBaseColor() const { return ColorF { 0.1, 0.15, 0.5 }; }
template<> double Material<M>::GetSinhaliteEffectiveness() const { return 1.0; }
template<> double Material<M>::GetDiggability() const { return 1.0; }

}
