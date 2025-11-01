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
    Vec2 speaker_position;
    Vec2 shop_position;
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
  static Parameters Generate(int level, bool is_game);

private:
  /// @brief 地形生成に使用する窓関数のようなもの。0〜1の実数に対し、端で0、それ以外で1に近い値を返す。
  /// @param x 入力値
  /// @return 出力値
  static double WindowFunction(double x);

  /// @brief 地形の元となるノードグリッドを作成する。
  /// @param size グリッドサイズ。
  /// @return ノードグリッド。
  static NodeGrid CreateNodeGrid(const Size& size);

  /// @brief 店用のノードグリッドを作成する。
  /// @return ノードグリッド。
  static NodeGrid CreateShopNodeGrid(int level);

  /// @brief チュートリアル用のノードグリッドを作成する。
  /// @return ノードグリッド。
  static NodeGrid CreateTutorialNodeGrid();

  /// @brief プレイヤーと敵の初期位置を作成する。
  /// @param terrain 地形クラス。
  /// @param enemy_distance プレイヤーと敵の最低距離。
  /// @return プレイヤーと敵の初期位置。
  static Optional<Positions> CreatePlayerAndEnemyPosition(const Terrain& terrain, double enemy_distance);

  /// @brief シンハライトの位置を作成する。
  /// @param node_grid ノードグリッド。
  /// @param count シンハライトの数。
  /// @return シンハライトの位置の配列。
  static Array<Point> CreateSinhalitePositions(const Terrain& terrain, int count);

private:

};


}
