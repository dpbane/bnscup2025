#include "access_map.hpp"

namespace bnscup2025::terrain {

AccessMap::AccessMap(const NodeGrid& node_grid, double threshold) :
  node_grid_(node_grid),
  threshold_(threshold),
  accessable_map_(node_grid.GetSize()),
  direction_map_(node_grid.GetSize()) {

  for (const auto& point : step(node_grid_.GetSize())) {
    UpdateAccessable(point);
  }
  for (const auto& point : step(node_grid_.GetSize())) {
    UpdateDirection(point);
  }
}

void AccessMap::Update(const NodeGrid& node_grid) {
  const auto updatable_map = CreateUpdatableMap(node_grid);
  node_grid_ = node_grid;

  for (const auto& point : step(node_grid_.GetSize())) {
    if (not updatable_map.Get(point)) continue;
    UpdateAccessable(point);
  }
  for (const auto& point : step(node_grid_.GetSize())) {
    if (not updatable_map.Get(point)) continue;
    UpdateDirection(point);
  }
}

Array<Array<Point>> AccessMap::CreateAreasArray() const {
  Array<Array<Point>> ret;

  // 訪問済みノード管理用配列
  GridPoints<bool> visited(node_grid_.GetSize());
  for (const auto& point : step(node_grid_.GetSize())) visited.Set(point, false);

  for (const auto& point : step(node_grid_.GetSize())) {
    // 訪問済みか、アクセス不可ならスキップ
    if (visited.Get(point)) continue;
    if (not accessable_map_.Get(point)) continue;

    // その点から連続領域を作成
    const auto area = CreateAreaFrom(point);
    for (const auto& p : area) visited.Set(p, true);
    ret.push_back(area);
  }

  return ret;
}

Array<Point> AccessMap::CreateAreaFrom(Point start_point) const {
  // 開始点がアクセス不可なら空配列を返す
  if (not accessable_map_.Get(start_point)) return {};

  // 訪問済みノード管理用配列
  GridPoints<bool> visited(node_grid_.GetSize());
  for (const auto& point : step(node_grid_.GetSize())) visited.Set(point, false);

  // 探索開始
  Array<Point> area;
  std::queue<Point> que;
  que.push(start_point);
  visited.Set(start_point, true);
  while (not que.empty()) {
    // キューから取り出し・訪問済みに設定
    const Point current = que.front();
    que.pop();
    area.push_back(current);

    // 隣接ノードを確認
    const auto direction = direction_map_.Get(current);
    auto CheckAndEnqueue = [&](const Point& neighbor) {
      if (visited.Get(neighbor)) return;
      que.push(neighbor);
      visited.Set(neighbor, true);
    };
    if (direction.up) CheckAndEnqueue(current.movedBy(0, -1));
    if (direction.down) CheckAndEnqueue(current.movedBy(0, 1));
    if (direction.left) CheckAndEnqueue(current.movedBy(-1, 0));
    if (direction.right) CheckAndEnqueue(current.movedBy(1, 0));
    if (direction.up_left) CheckAndEnqueue(current.movedBy(-1, -1));
    if (direction.up_right) CheckAndEnqueue(current.movedBy(1, -1));
    if (direction.down_left) CheckAndEnqueue(current.movedBy(-1, 1));
    if (direction.down_right) CheckAndEnqueue(current.movedBy(1, 1));
  }

  return area;
}

GridPoints<bool> AccessMap::CreateUpdatableMap(const NodeGrid& node_grid) const {
  GridPoints<bool> ret(node_grid_.GetSize());
  for (const auto& point : step(node_grid_.GetSize())) {
    ret.Set(point, node_grid_.Get(point).density != node_grid.Get(point).density);
  }
  return ret;
}

void AccessMap::UpdateAccessable(Point point) {
  if (accessable_map_.Get(point)) return;  // 既にアクセス可能なら更新不要

  const double t = node_grid_.Get(point.movedBy(0, -1)).density;
  const double l = node_grid_.Get(point.movedBy(-1, 0)).density;
  const double c = node_grid_.Get(point).density;
  const double r = node_grid_.Get(point.movedBy(1, 0)).density;
  const double b = node_grid_.Get(point.movedBy(0, 1)).density;

  // 自身が壁の中だったらアクセス不可
  if (c <= threshold_) {
    accessable_map_.Set(point, false);
    return;
  }

  // 媒介変数tを求める関数
  auto CalcT = [this](double v1, double v2) -> double {
    if (v1 == v2) return 0.5;
    return (threshold_ - v1) / (v2 - v1);
  };

  // 格子点付近に壁がある場合はアクセス不可
  const double p_top = t <= threshold_ ? CalcT(c, t) : 1.0;
  const double p_bottom = b <= threshold_ ? CalcT(c, b) : 1.0;
  const double p_left = l <= threshold_ ? CalcT(c, l) : 1.0;
  const double p_right = r <= threshold_ ? CalcT(c, r) : 1.0;
  if ((p_top + p_bottom < kCharacterRadius * 2.0) ||
    (p_left + p_right < kCharacterRadius * 2.0)) {
    accessable_map_.Set(point, false);
    return;
  }

  // いずれでもない場合はアクセス可能
  accessable_map_.Set(point, true);
}

void AccessMap::UpdateDirection(Point point) {
  // アクセス不可なら方向情報は空にする
  if (accessable_map_.Get(point) == false) {
    direction_map_.Set(point, AccessableDirection {});
    return;
  }

  auto direction = direction_map_.Get(point);

  // 上下左右を確認
  direction.up = accessable_map_.Get(point.movedBy(0, -1));
  direction.down = accessable_map_.Get(point.movedBy(0, 1));
  direction.left = accessable_map_.Get(point.movedBy(-1, 0));
  direction.right = accessable_map_.Get(point.movedBy(1, 0));

  // 斜め判定用の関数
  auto CanAccessDiagonal = [this](const Point& from, const Point& to, const Point& ext1, const Point& ext2) -> bool {
    // 移動可能性で大雑把な判定
    if (accessable_map_.Get(to) == false) return false;
    if (accessable_map_.Get(ext1) || accessable_map_.Get(ext2)) return true;

    // この時点で鞍点であることが確定するので、分離型かを判定
    const double v_from = node_grid_.Get(from).density;
    const double v_to = node_grid_.Get(to).density;
    const double v_ext1 = node_grid_.Get(ext1).density;
    const double v_ext2 = node_grid_.Get(ext2).density;
    const double mean = (v_from + v_to + v_ext1 + v_ext2) / 4;
    if (mean <= threshold_) return false;  // 分離型でないならアクセス不可

    // 分離型鞍点であることが確定したので、対角線における壁の幅を計算

    // 媒介変数tを求める関数
    auto CalcT = [this](double v1, double v2) -> double {
      if (v1 == v2) return 0.5;
      return (threshold_ - v1) / (v2 - v1);
    };

    const double p_ext1 = CalcT(mean, v_ext1);
    const double p_ext2 = CalcT(mean, v_ext2);
    if (p_ext1 + p_ext2 < kCharacterRadius * 2.0) return false;

    return true;
  };

  // 斜め方向を確認
  if (not direction.up_left) direction.up_left = CanAccessDiagonal(point, point.movedBy(-1, -1), point.movedBy(-1, 0), point.movedBy(0, -1));
  if (not direction.up_right) direction.up_right = CanAccessDiagonal(point, point.movedBy(1, -1), point.movedBy(1, 0), point.movedBy(0, -1));
  if (not direction.down_left) direction.down_left = CanAccessDiagonal(point, point.movedBy(-1, 1), point.movedBy(-1, 0), point.movedBy(0, 1));
  if (not direction.down_right) direction.down_right = CanAccessDiagonal(point, point.movedBy(1, 1), point.movedBy(1, 0), point.movedBy(0, 1));
  direction_map_.Set(point, direction);

  // 相手側の方向情報も更新
  if (direction.up) {
    auto temp = direction_map_.Get(point.movedBy(0, -1));
    temp.down = true;
    direction_map_.Set(point.movedBy(0, -1), temp);
  }
  if (direction.down) {
    auto temp = direction_map_.Get(point.movedBy(0, 1));
    temp.up = true;
    direction_map_.Set(point.movedBy(0, 1), temp);
  }
  if (direction.left) {
    auto temp = direction_map_.Get(point.movedBy(-1, 0));
    temp.right = true;
    direction_map_.Set(point.movedBy(-1, 0), temp);
  }
  if (direction.right) {
    auto temp = direction_map_.Get(point.movedBy(1, 0));
    temp.left = true;
    direction_map_.Set(point.movedBy(1, 0), temp);
  }
  if (direction.up_left) {
    auto temp = direction_map_.Get(point.movedBy(-1, -1));
    temp.down_right = true;
    direction_map_.Set(point.movedBy(-1, -1), temp);
  }
  if (direction.up_right) {
    auto temp = direction_map_.Get(point.movedBy(1, -1));
    temp.down_left = true;
    direction_map_.Set(point.movedBy(1, -1), temp);
  }
  if (direction.down_left) {
    auto temp = direction_map_.Get(point.movedBy(-1, 1));
    temp.up_right = true;
    direction_map_.Set(point.movedBy(-1, 1), temp);
  }
  if (direction.down_right) {
    auto temp = direction_map_.Get(point.movedBy(1, 1));
    temp.up_left = true;
    direction_map_.Set(point.movedBy(1, 1), temp);
  }
}

}
