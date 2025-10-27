#pragma once

#include "node_grid.hpp"
#include "terrain.hpp"

namespace bnscup2025::terrain {

class MapGenerator {
public:
  struct Parameters {
    Terrain terrain;
    Vec2 player_position;
    Vec2 enemy_position;
    Vec2 exit_position;
  };

private:
  struct Positions {
    Vec2 player_position;
    Vec2 enemy_position;
    Vec2 exit_position;
  };

private:
  MapGenerator() = delete;
  MapGenerator(const MapGenerator&) = delete;
  MapGenerator& operator=(const MapGenerator&) = delete;

public:
  static Parameters Generate(int level);

private:
  /// @brief 地形生成に使用する窓関数のようなもの。0〜1の実数に対し、端で0、それ以外で1に近い値を返す。
  /// @param x 入力値
  /// @return 出力値
  static double WindowFunction(double x);

  /// @brief 地形の元となるノードグリッドを作成する。
  /// @param size グリッドサイズ。
  /// @param seed シード値。
  /// @param octaves ノイズの設定値その1。何オクターブまで使用するか。
  /// @param persistence ノイズの設定値その2。大まかな波の形状を決める。
  /// @param scale スケール値。大きいとより狭い範囲で変化が起こる。
  /// @return ノードグリッド。
  static NodeGrid CreateNodeGrid(const Size& size, uint64 seed, int octaves, double persistence, double scale);

  /// @brief プレイヤーと敵の初期位置を作成する。
  /// @param terrain 地形クラス。
  /// @return プレイヤーと敵の初期位置。
  static Positions CreatePlayerAndEnemyPosition(const Terrain& terrain);

  /// @brief シンハライトの位置を作成する。
  /// @param node_grid ノードグリッド。
  /// @param count シンハライトの数。
  /// @return シンハライトの位置の配列。
  static Array<Point> CreateSinhalitePositions(const NodeGrid& node_grid, int count);

  /// @brief 連続する領域のうち最も大きいものを取得する。
  /// @param node_grid ノードグリッド。
  /// @return 最も大きい領域を構成するノードの位置の配列。
  static Array<Point> GetBiggestArea(const NodeGrid& node_grid);


private:

};


}
