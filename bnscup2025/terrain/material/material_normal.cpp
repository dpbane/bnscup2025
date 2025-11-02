#include "material.hpp"

namespace bnscup2025::terrain {

constexpr MaterialEnum M = MaterialEnum::Normal;

template<> ColorF Material<M>::GetBaseColor(int level) const {
  const ColorF level1_color = ColorF { 0.1, 0.2, 0.6 };
  const ColorF level12_color = ColorF { 0.2, 0.05, 0.09 };
  int lvl = Clamp(level, 1, 12);
  double t = (lvl - 1) / 11.0;
  return level1_color * (1.0 - t) + level12_color * t;
}
template<> double Material<M>::GetSinhaliteEffectiveness() const { return 1.0; }
template<> double Material<M>::GetDiggability() const { return 1.0; }

}
