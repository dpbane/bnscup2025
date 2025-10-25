#include "material.hpp"

namespace bnscup2025::terrain {

constexpr MaterialEnum M = MaterialEnum::Normal;

template<> ColorF Material<M>::GetBaseColor() const { return ColorF { 0.2, 0.3, 1.0 }; }
template<> double Material<M>::GetSinhaliteEffectiveness() const { return 1.0; }
template<> double Material<M>::GetDiggability() const { return 1.0; }

}
