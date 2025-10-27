#pragma once


namespace bnscup2025::player {

/*
  グレードアップ一覧

  [エネルギー最大値アップ] : これのみレベル制でなく、実際の数値を増減させる
  [護身術【伸】] : シフト距離増加
  [護身術【心】] : コスト減少
  [護身術【進】] : 移動速度増加
  [護身術【慎】] : コスト減少
  [護身術【診】] : コスト減少
  [神] : エンディング条件

*/

enum class PowerGradeItem : uint8 {
  MaxEnergy,
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
