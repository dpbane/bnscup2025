#pragma once

#include "power_grade.hpp"

namespace bnscup2025::player {

class GradeValueConverter {
public:
  GradeValueConverter(const PowerGrade& power_grade) :
    power_grade_(power_grade) {
  }

  /// @brief 最大エネルギー値を取得する。
  double GetMaxEnergy() const;

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
