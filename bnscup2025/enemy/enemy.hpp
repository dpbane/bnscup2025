#pragma once

#include "terrain/terrain.hpp"
#include "camera/camera.hpp"
#include "player/player.hpp"

namespace bnscup2025::enemy {

struct EnemyParameters {
  double sound_hear_radius;
  double view_radius;
  double view_fov_cos;
  double prowl_speed;
  double to_sound_speed;
  double pursuit_speed;
};

class Enemy {
private:
  enum class State {
    // 徘徊
    Prowl,
    ToSound,
    Pursuit
  };

public:
  Enemy(const camera::Camera& camera,
    const terrain::Terrain& terrain,
    const player::Player& player,
    const Vec2& pos,
    const EnemyParameters& parameters);
  ~Enemy();

  void Update();
  void Render() const;
  void RenderUI() const;

  bool IsPlayerCaught() const;

private:
  void OnProwlUpdate();
  void OnToSoundUpdate();
  void OnPursuitUpdate();

  Optional<Point> CalcNearestValidPoint(const Vec2& pos) const;
  Vec2 CalcDirectionFromPathMap() const;
  bool IsArrivedAtTarget() const;
  bool HasHeardSound() const;
  bool CanSeePlayer(double range, double fov_cos) const;

private:
  bool CanPassThrough(const Vec2& from, const Vec2& to) const;
  Point GetNextPointFromPathMap(const Point& current_point) const;

private:
  void BeginThread(Point target_point);
  void EndThread();
  void ThreadMain(Point target_point);

  bool IsCalculatingPath() const { return thread_run_flag_.load(); }


private:
  const camera::Camera& camera_;
  const terrain::Terrain& terrain_;
  const player::Player& player_;
  const EnemyParameters parameters_;

  // 最短経路探索まわりの変数
  Optional<terrain::AccessMap> access_map_copy_;
  Optional<terrain::GridPoints<double>> cost_map_;
  std::atomic_bool thread_run_flag_ { false };
  std::thread thread_;
  std::mutex cost_map_mutex_;

  // 基本変数
  Vec2 position_;
  Vec2 velocity_ { 0.0, 0.0 };
  Vec2 direction_face_ { 0.0, -1.0 };
  State state_ { State::Prowl };

  // 追跡モード
  Optional<Vec2> last_saw_position_;  // 最後にプレイヤーを発見した位置

  double await_timer_ { 0.0 };

  double ui_alpha_ { 0.0 };


};

}
