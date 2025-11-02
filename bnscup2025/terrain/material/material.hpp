#pragma once

namespace bnscup2025::terrain {

class IMaterial {
public:
  virtual ~IMaterial() = default;

  virtual ColorF GetBaseColor(int level) const = 0;  // 基本色
  virtual double GetSinhaliteEffectiveness() const = 0;  // シンハライトが近くにある時に色が変わる程度の強さ。1が標準。
  virtual double GetDiggability() const = 0;  // 掘削しやすさ。1が標準。
};

enum class MaterialEnum {
  Normal,
  Bounds,
  HardRock
};

template<MaterialEnum M>
class Material : public IMaterial {
public:
  ~Material() override = default;
  ColorF GetBaseColor(int level) const override;
  double GetSinhaliteEffectiveness() const override;
  double GetDiggability() const override;
};


}
