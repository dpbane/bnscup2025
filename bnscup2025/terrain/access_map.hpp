#pragma once

#include "node_grid.hpp"

namespace bnscup2025::terrain {

/// @brief ノードグリッド上の点にプレイヤーや敵が移動可能か？隣のグリッドへアクセス可能か？を示すマップ。
class AccessMap {
public:
  struct AccessableDirection {
    bool up : 1 {false};
    bool down : 1 {false};
    bool left : 1 {false};
    bool right : 1 {false};
    bool up_left : 1 {false};
    bool up_right : 1 {false};
    bool down_left : 1 {false};
    bool down_right : 1 {false};
  };

public:
  /// @brief コンストラクタ。
  /// @param node_grid ノードグリッド。
  /// @param threshold 閾値。
  AccessMap(const NodeGrid& node_grid, double threshold);

  /// @brief ノードグリッドの変更に応じてマップを更新する。
  /// @param node_grid 更新後のノードグリッド。
  void Update(const NodeGrid& node_grid);

  /// @brief 連続する領域の配列を取得する。
  /// @return 連続する領域の配列。
  Array<Array<Point>> CreateAreasArray() const;

  /// @brief 指定した点を含む連続する領域を取得する。指定した点がアクセス不可の場合は空配列を返す。
  /// @param start_point 開始点。
  /// @return 連続する領域。
  Array<Point> CreateAreaFrom(Point start_point) const;

  /// @brief アクセス可能マップを取得する。
  /// @return アクセス可能マップ。 
  GridPoints<bool> GetAccessableMap() const { return accessable_map_; }

  /// @brief アクセス可能な方向を示すマップを取得する。
  /// @return アクセス可能な方向を示すマップ。
  GridPoints<AccessableDirection> GetDirectionMap() const { return direction_map_; }

private:
  /// @brief ノードグリッドの変更に応じて更新が必要な箇所を示すマップを作成する。
  /// @param node_grid 更新後のノードグリッド。
  /// @return 更新が必要な箇所を示すマップ。
  GridPoints<bool> CreateUpdatableMap(const NodeGrid& node_grid) const;

  /// @brief 指定した位置のアクセス可能情報を更新する。
  /// @param point 更新する位置。
  void UpdateAccessable(Point point);

  /// @brief 指定した位置のアクセス可能な方向情報を更新する。
  /// @param point 更新する位置。
  void UpdateDirection(Point point);

private:
  NodeGrid node_grid_;
  const double threshold_;

  GridPoints<bool> accessable_map_;
  GridPoints<AccessableDirection> direction_map_;


};

}
