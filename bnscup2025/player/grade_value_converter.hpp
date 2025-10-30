#pragma once

#include "power_grade.hpp"

namespace bnscup2025::player {

/*
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

class GradeValueConverter {
public:
  GradeValueConverter(const PowerGrade& power_grade) :
    power_grade_(power_grade) {
  }

  /// @brief 最大エネルギー値を取得する。
  double GetMaxEnergy() const;

  /// @brief エネルギー回復速度を取得する。
  double GetEnergyRegen() const;

  /// @brief 採掘クールダウンを取得する。
  double GetDigCooldown() const;

  /// @brief 採掘範囲を取得する。
  double GetDigRange() const;

  /// @brief 視界角度を取得する。
  double GetViewAngle() const;

  /// @brief 移動速度を取得する。
  double GetMoveSpeed() const;

  /// @brief 護身術【伸】のシフト距離を取得する。
  double GetNobiruShiftDistance() const;

  /// @brief 護身術【心】の発動コストを取得する。
  double GetKokoroBeginCost() const;

  /// @brief 護身術【心】の維持コスト（秒あたり）を取得する。
  double GetKokoroMaintainCost() const;

  /// @brief 護身術【進】の移動速度を取得する。
  double GetSususmuSpeed() const;

  /// @brief 護身術【慎】の発動コストを取得する。
  double GetTsutsuStartCost() const;

  /// @brief 護身術【慎】の維持コスト（秒あたり）を取得する。
  double GetTsutsuMaintainCost() const;



  /// @brief 護身術【伸】を所持しているかを取得する。
  bool HasNobiru() const;

  /// @brief 護身術【心】を所持しているかを取得する。
  bool HasKokoro() const;

  /// @brief 護身術【進】を所持しているかを取得する。
  bool HasSusumu() const;

  /// @brief 護身術【慎】を所持しているかを取得する。
  bool HasTsutsu() const;

  /// @brief 護身術【診】を所持しているかを取得する。 
  bool HasMiru() const;

  /// @brief 「神」を所持しているかを取得する。
  bool HasGod() const;

private:
  const PowerGrade& power_grade_;

};

}
