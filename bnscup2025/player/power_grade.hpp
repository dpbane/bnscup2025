#pragma once


namespace bnscup2025::player {

/*
  グレートアップ一覧

  [エネルギー最大値アップ] : これのみレベル制でなく、実際の数値を増減させる
  [エネルギー回復速度アップ]
  [視野角度アップ]
  [視野距離アップ]
  [採掘速度アップ]
  [護身術【針】] : コスト減少
  [護身術【進】] : コスト減少
  [護身術【心】] : コスト減少
  [護身術【診】] : コスト減少
  [神] : エンディング条件

*/

struct PowerGrade {
  int max_energy;
  uint8 regenerate;
  uint8 digging;
  uint8 hari;
  uint8 susumu;
  uint8 kokoro;
  uint8 miru;
  bool god;
};

}
