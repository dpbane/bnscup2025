#pragma once


namespace bnscup2025::player {

enum class PowerGradeItem : uint8 {
  MaxEnergy,
  EnergyRegen,
  DigSpeed,
  DigMight,
  DigDistance,
  ViewAngle,
  ViewDistance,
  MoveSpeed,
  Nobiru,
  Kokoro,
  Susumu,
  Tsutsu,
  Miru,
  God,
  Reroll
};

/// @brief プレイヤーの強化状態を管理するクラス。
class PowerGrade {
public:
  PowerGrade();

  uint8& operator[](PowerGradeItem item);
  const uint8& operator[](PowerGradeItem item) const;

private:
  std::array<uint8, static_cast<size_t>(PowerGradeItem::Reroll) + 1> grade_levels_;
};


}
