#include "stdafx.h"
#include "item_info_factory.hpp"

namespace bnscup2025::npc {

ItemInfo ItemInfoFactory::Get(player::PowerGradeItem item) {
  switch (item) {
    case player::PowerGradeItem::MaxEnergy:
      return ItemInfo {
        U"意気軒昂",
        U"エネルギーの最大値が増加する。\nエネルギーはゴシン術使用時に消費される。",
        Array<int> { 4, 5, 6, 7, 8, 9, 10, 11 }
      };

    case player::PowerGradeItem::EnergyRegen:
      return ItemInfo {
        U"高速回復",
        U"エネルギー回復速度が増加する。\nエネルギーはゴシン術使用時に消費される。\n疲労状態の回復速度には影響しない。",
        Array<int> { 6, 8, 10 }
      };

    case player::PowerGradeItem::DigSpeed:
      return ItemInfo {
        U"高速採掘",
        U"掘削後のクールダウン時間が短縮される。",
        Array<int> { 6, 8, 10 }
      };

    case player::PowerGradeItem::DigMight:
      return ItemInfo {
        U"広域掘削",
        U"一度の採掘で影響を与える範囲が広くなる。",
        Array<int> { 6, 8, 10 }
      };

    case player::PowerGradeItem::DigDistance:
      return ItemInfo {
        U"遠距離掘削",
        U"掘削可能な距離が伸びる。",
        Array<int> { 6, 8, 10 }
      };

    case player::PowerGradeItem::ViewAngle:
      return ItemInfo {
        U"視野拡大",
        U"視野角が広くなる。",
        Array<int> { 5, 7, 9 }
      };

    case player::PowerGradeItem::ViewDistance:
      return ItemInfo {
        U"視力増強",
        U"より遠くまで見えるようになる。",
        Array<int> { 7, 9, 11 }
      };

    case player::PowerGradeItem::MoveSpeed:
      return ItemInfo {
        U"高速移動",
        U"移動速度が増加する。",
        Array<int> { 6, 8, 10 }
      };

    case player::PowerGradeItem::Nobiru:
      return ItemInfo {
        U"ゴシン術【伸】",
        U"右クリック/LBを押しながら移動しようとすると、自身は移動せずに視点のみ少し移動する。\n"
        U"エネルギーを消費しないが、回復もしない。レベル上昇で移動距離増加。\n"
        U"ゴシン術を複数習得している場合、マウスホイール / LT / RTでゴシン術を切り替える。",
        Array<int> { 7, 8, 10 }
      };

    case player::PowerGradeItem::Kokoro:
      return ItemInfo {
        U"ゴシン術【心】",
        U"右クリック/LBで使用。エネルギーを消費して視界外の情報が分かる。\n"
        U"長押しするとエネルギーを消費しながら継続。レベル上昇でエネルギー消費量減少。\n"
        U"ゴシン術を複数習得している場合、マウスホイール / LT / RTでゴシン術を切り替える。",
        Array<int> { 12, 14, 15 }
      };

    case player::PowerGradeItem::Susumu:
      return ItemInfo {
        U"ゴシン術【進】",
        U"右クリック/LBを押している間、音が発生するが通常よりも速く移動できる。\n"
        U"使用中は常にエネルギーを消費。レベル上昇で移動速度増加。\n"
        U"ゴシン術を複数習得している場合、マウスホイール / LT / RTでゴシン術を切り替える。",
        Array<int> { 12, 14, 15 }
      };

    case player::PowerGradeItem::Tsutsu:
      return ItemInfo {
        U"ゴシン術【慎】",
        U"右クリック/LBを押している間、移動速度が大幅に低下するが鬼に発見されなくなる。\n"
        U"既に発見されている場合は効果が無い。レベル上昇でエネルギー消費量減少。\n"
        U"ゴシン術を複数習得している場合、マウスホイール / LT / RTでゴシン術を切り替える。",
        Array<int> { 10, 12, 14 }
      };

    case player::PowerGradeItem::Miru:
      return ItemInfo {
        U"ゴシン術【診】",
        U"鬼が警戒状態や発見状態になったとき、それを検知できる。\n"
        U"常に効果を発揮するが、購入時にエネルギー最大値が初期値の半分だけ減少する。",
        Array<int> { 40 }
      };

    case player::PowerGradeItem::God:
      return ItemInfo {
        U"神",
        U"神",
        Array<int> { 100 }
      };

    case player::PowerGradeItem::Reroll:
      return ItemInfo {
        U"再抽選",
        U"強化項目を再抽選する。",
        Array<int> { 1 }
      };

  }

  assert(false);
  return ItemInfo {};
}

}
