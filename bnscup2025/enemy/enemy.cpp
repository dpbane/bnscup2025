#include "enemy.hpp"

#include "render/blend_mode.hpp"
#include "render/lightbloom.hpp"
#include "render/chara_renderer.hpp"

namespace bnscup2025::enemy {

Enemy::Enemy(const camera::Camera& camera,
  const terrain::Terrain& terrain,
  const player::Player& player,
  const Vec2& pos,
  const EnemyParameters& parameters) :
  camera_(camera),
  terrain_(terrain),
  player_(player),
  parameters_(parameters),
  position_(pos) {
}

Enemy::~Enemy() {
  EndThread();
}

void Enemy::Update() {

  {
    std::lock_guard lock { cost_map_mutex_ };
    switch (state_) {
      case State::Prowl:
        OnProwlUpdate();
        break;

      case State::ToSound:
        OnToSoundUpdate();
        break;

      case State::Pursuit:
        OnPursuitUpdate();
        break;
    }
  }

  AvoidWall();

  direction_face_ = velocity_.isZero() ? direction_face_ : velocity_.normalized();
  position_ += velocity_ * Scene::DeltaTime();
  position_ = terrain_.PushbackService(Circle { position_, kCharacterRadius });
}

void Enemy::Render() const {
  const auto& lightbloom = render::LightBloom::GetInstance();
  const ColorF color_edge { 0.9, 0.2, 0.3 };
  const ColorF color_body { 0.05, 0.01, 0.02 };
  render::CharaRenderer::Render(camera_, position_, direction_face_, color_body, color_edge, 1.0, 2.0, 2.0);

  // デバッグ用：状態表示
  switch (state_) {
    case State::Prowl: Print << U"Prowl"; break;
    case State::ToSound: Print << U"ToSound"; break;
    case State::Pursuit: Print << U"Pursuit"; break;
  }

}

void Enemy::OnProwlUpdate() {
  // 目視したら追跡モードに遷移
  if (CanSeePlayer(parameters_.view_radius, 0.5)) {
    state_ = State::Pursuit;
    return;
  }


  // 音を聞いたら計算とともに遷移
  if (HasHeardSound()) {
    if (const auto sound_position = CalcNearestValidPoint(*player_.GetSoundPosition())) {
      BeginThread(*sound_position);
      state_ = State::ToSound;
      return;
    }
  }

  // 次の経路が不明な場合
  if (not cost_map_ && not IsCalculatingPath()) {
    // 最も近い有効な点を探す
    const auto point = CalcNearestValidPoint(position_);
    if (not point) return;

    // その位置から連続する領域を取得する
    const auto area = terrain_.GetAccessMap().CreateAreaFrom(*point);
    if (area.isEmpty()) return;

    // 領域からランダムに目的地を選び、最短経路探索スレッドを開始する
    // 目的地はプレイヤーから近すぎず、現在位置からも近すぎない点を選ぶ
    while (true) {
      const auto& target_point = area.choice();
      if (player_.GetPosition().distanceFrom(target_point) < 2.0) continue;
      if (position_.distanceFrom(target_point) < 5.0) continue;
      BeginThread(target_point);
      break;
    }

  }

  // 最短経路探索が完了している場合
  if (cost_map_) {
    Vec2 direction = CalcDirectionFromPathMap();
    velocity_ = direction.normalized() * parameters_.prowl_speed;
    if (IsArrivedAtTarget()) cost_map_ = none;  // 目的地に到着したらクリア、再計算を促す
  }
  else {
    velocity_ = Vec2 { 0.0, 0.0 };
  }
}

void Enemy::OnToSoundUpdate() {
  // 目視したら追跡モードに遷移
  if (CanSeePlayer(parameters_.view_radius, 0.5)) {
    state_ = State::Pursuit;
    return;
  }

  // 音を聞いたら再計算
  if (HasHeardSound()) {
    if (const auto sound_position = CalcNearestValidPoint(*player_.GetSoundPosition())) {
      BeginThread(*sound_position);
    }
  }

  // 次の経路が不明な場合、1秒待機して徘徊モードに戻る
  if (not cost_map_ && not IsCalculatingPath()) {
    await_timer_ += Scene::DeltaTime();
    if (await_timer_ >= 1.0) {
      state_ = State::Prowl;
      await_timer_ = 0.0;
      return;
    }
  }
  else {
    await_timer_ = 0.0;
  }

  // 最短経路探索が完了している場合
  if (cost_map_) {
    Vec2 direction = CalcDirectionFromPathMap();
    velocity_ = direction.normalized() * parameters_.to_sound_speed;
    if (IsArrivedAtTarget()) cost_map_ = none;  // 目的地に到着したらクリア
  }
  else {
    velocity_ = Vec2 { 0.0, 0.0 };
  }

}

void Enemy::OnPursuitUpdate() {
  Print << last_saw_position_;
  Print << cost_map_.has_value();
  if (CanSeePlayer(parameters_.view_radius, -1.0)) {
    // プレイヤーが見えている場合、直接向かう
    const Vec2 to_player = (player_.GetPosition() - position_).normalized();
    velocity_ = to_player * parameters_.pursuit_speed;
    last_saw_position_ = player_.GetPosition();
    cost_map_.reset();
    await_timer_ = 0.0;
    return;
  }

  if (HasHeardSound()) {
    if (const auto sound_position = CalcNearestValidPoint(*player_.GetSoundPosition())) {
      last_saw_position_.reset();
      BeginThread(*sound_position);
    }
  }

  if (not cost_map_ && not IsCalculatingPath()) {
    if (last_saw_position_) {
      BeginThread(*CalcNearestValidPoint(*last_saw_position_));
    }
    else {
      await_timer_ += Scene::DeltaTime();
      if (await_timer_ >= 1.0) {
        state_ = State::Prowl;
        return;
      }
    }
  }
  else {
    await_timer_ = 0.0;
  }

  if (cost_map_) {
    Vec2 direction = CalcDirectionFromPathMap();
    velocity_ = direction.normalized() * parameters_.pursuit_speed;
    if (IsArrivedAtTarget()) {
      cost_map_ = none;
      last_saw_position_.reset();
    }
  }
  else {
    velocity_ = Vec2 { 0.0, 0.0 };
  }
}

void Enemy::AvoidWall() {
  // 壁にぶつかりにくくする調整
  Vec2 test_position = terrain_.PushbackService(Circle { position_, kCharacterRadius * 2.0 });
  Vec2 test_direction = (test_position - position_).normalized();
  double speed = velocity_.length();
  velocity_ = (velocity_.normalized() + test_direction * 0.2).normalized() * speed;
}

Optional<Point> Enemy::CalcNearestValidPoint(const Vec2& pos) const {
  const auto& accessable_map = terrain_.GetAccessMap().GetAccessableMap();
  const Point point_nearest = {
    static_cast<int32>(std::round(pos.x)),
    static_cast<int32>(std::round(pos.y))
  };
  if (accessable_map.Get(point_nearest)) return point_nearest;

  // 次に探索する候補点
  constexpr Point candidates[] = {
    { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 },
    { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 }
  };
  double distance = 10.0;
  Optional<Point> ret;
  for (const auto& candidate : candidates) {
    const Point check_point = point_nearest + candidate;
    if (accessable_map.Get(check_point)) {
      const double check_distance = (check_point - pos).lengthSq();
      if (check_distance < distance) {
        distance = check_distance;
        ret = check_point;
      }
    }
  }
  return ret;
}

Vec2 Enemy::CalcDirectionFromPathMap() const {
  Optional<Point> current_point = CalcNearestValidPoint(position_);
  if (not current_point) return Vec2 {};
  if (not cost_map_) return Vec2 {};

  constexpr Point candidates[] = {
    {0, 0}, { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 },
    { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 }
  };

  double min_cost = std::numeric_limits<double>::infinity();
  Point next_point = *current_point;
  for (const auto& candidate : candidates) {
    const Point check_point = *current_point + candidate;
    const double check_cost = cost_map_->Get(check_point);
    if (check_cost < min_cost) {
      min_cost = check_cost;
      next_point = check_point;
    }
  }
  return Vec2 { static_cast<double>(next_point.x - position_.x),
                 static_cast<double>(next_point.y - position_.y) }.normalized();
}

bool Enemy::IsArrivedAtTarget() const {
  Optional<Point> current_point = CalcNearestValidPoint(position_);
  if (not current_point) return false;
  if (not cost_map_) return false;
  return cost_map_->Get(*current_point) == 0.0;
}

bool Enemy::HasHeardSound() const {
  const auto& sound_position = player_.GetSoundPosition();
  if (not sound_position) return false;
  const double distance = position_.distanceFrom(*sound_position);
  return distance <= parameters_.sound_hear_radius;
}

bool Enemy::CanSeePlayer(double range, double fov_cos) const {
  const Vec2 to_player = player_.GetPosition() - position_;
  const double length = to_player.length();

  // 距離が遠すぎたら見えない
  if (length > range) return false;

  // 遮られていたら見えない
  const double theta = atan2(kCharacterRadius, length);
  const auto point1 = terrain_.CalcLineCollisionPoint(position_, to_player.normalized(), range);
  const auto point2 = terrain_.CalcLineCollisionPoint(position_, to_player.rotated(theta).normalized(), range);
  const auto point3 = terrain_.CalcLineCollisionPoint(position_, to_player.rotated(-theta).normalized(), range);
  if (point1 && point1->distanceFrom(position_) < length - kCharacterRadius &&
    point2 && point2->distanceFrom(position_) < length - kCharacterRadius &&
    point3 && point3->distanceFrom(position_) < length - kCharacterRadius) {
    return false;
  }

  return to_player.normalized().dot(direction_face_) >= fov_cos;
}

void Enemy::BeginThread(Point target_point) {
  EndThread();
  thread_run_flag_.store(true);
  access_map_copy_.emplace(terrain_.GetAccessMap());
  thread_ = std::thread(&Enemy::ThreadMain, this, target_point);
}

void Enemy::EndThread() {
  thread_run_flag_ = false;
  if (thread_.joinable()) thread_.join();
}

void Enemy::ThreadMain(Point target_point) {
  const auto& direction_map = access_map_copy_->GetDirectionMap();

  // 探索用変数の初期化
  HashSet<Point> visited;
  HashSet<Point> que;
  terrain::GridPoints<double> cost_map(direction_map.GetSize());
  for (const auto& pos : step(direction_map.GetSize())) cost_map.Set(pos, std::numeric_limits<double>::infinity());
  cost_map.Set(target_point, 0.0);

  auto Enqueue = [&](bool flag, const Point& point, double cost) {
    if (flag && not visited.contains(point)) {
      if (cost < cost_map.Get(point)) cost_map.Set(point, cost);
      que.insert(point);
    }
  };

  auto Check = [&](const Point& point) {
    visited.insert(point);
    que.erase(point);

    const double current_cost = cost_map.Get(point);
    const auto direction = direction_map.Get(point);
    Enqueue(direction.up, point.movedBy(0, -1), current_cost + 1.0);
    Enqueue(direction.down, point.movedBy(0, 1), current_cost + 1.0);
    Enqueue(direction.left, point.movedBy(-1, 0), current_cost + 1.0);
    Enqueue(direction.right, point.movedBy(1, 0), current_cost + 1.0);
    Enqueue(direction.up_left, point.movedBy(-1, -1), current_cost + Sqrt(2));
    Enqueue(direction.up_right, point.movedBy(1, -1), current_cost + Sqrt(2));
    Enqueue(direction.down_left, point.movedBy(-1, 1), current_cost + Sqrt(2));
    Enqueue(direction.down_right, point.movedBy(1, 1), current_cost + Sqrt(2));
  };

  Check(target_point);
  while (thread_run_flag_ && not que.empty()) {
    // queのうち最小コストの点を探す
    Point current_point {};
    double current_cost = std::numeric_limits<double>::infinity();
    for (const auto& point : que) {
      const double point_cost = cost_map.Get(point);
      if (point_cost < current_cost) {
        current_cost = point_cost;
        current_point = point;
      }
    }
    Check(current_point);
  }
  if (thread_run_flag_) {
    std::lock_guard lock { cost_map_mutex_ };
    cost_map_ = std::move(cost_map);
  }
  thread_run_flag_ = false;

  return;
}

}
