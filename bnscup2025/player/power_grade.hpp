#pragma once


namespace bnscup2025::player {

/*
  グレードアップ一覧

  [エネルギー最大値増加] : 最大エネルギー増加
  [エネルギー回復速度増加] : エネルギー回復速度アップ
  [採掘速度増加] : 採掘クールダウン減少
  [採掘威力増加] : 1回の採掘で掘れる範囲増加
  [視界拡大] : 視界角度増加
  [移動速度増加] : 移動速度増加
  [護身術【伸】] : シフト距離増加
  [護身術【心】] : コスト減少
  [護身術【進】] : 移動速度増加
  [護身術【慎】] : コスト減少
  [護身術【診】] : コスト減少
  [神] : エンディング条件

*/

enum class PowerGradeItem : uint8 {
  MaxEnergy,
  EnergyRegen,
  DigSpeed,
  DigMight,
  ViewAngle,
  MoveSpeed,
  Nobiru,
  Kokoro,
  Susumu,
  Tsutsu,
  Miru,
  God
};

/// @brief プレイヤーの強化状態を管理するクラス。
class PowerGrade {
public:
  PowerGrade();

  uint8& operator[](PowerGradeItem item);
  const uint8& operator[](PowerGradeItem item) const;

private:
  std::array<uint8, static_cast<size_t>(PowerGradeItem::God) + 1> grade_levels_;
};


}
