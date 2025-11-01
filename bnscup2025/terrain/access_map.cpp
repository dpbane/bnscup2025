#include "access_map.hpp"

#include "pushback_service.hpp"

namespace bnscup2025::terrain {

AccessMap::AccessMap(const NodeGrid& node_grid, const MarchingSquares& marching_squares, double threshold) :
  node_grid_(node_grid),
  marching_squares_(marching_squares),
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

void AccessMap::Update(const GridPoints<bool>& update_node) {
  for (const auto& point : step(node_grid_.GetSize())) {
    if (not update_node.Get(point)) continue;
    UpdateAccessable(point);
  }
  for (const auto& point : step(node_grid_.GetSize())) {
    if (not update_node.Get(point)) continue;
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

void AccessMap::UpdateAccessable(Point point) {
  if (accessable_map_.Get(point)) return;  // 既にアクセス可能なら更新不要

  const double c = node_grid_.Get(point).density;

  // 自身が壁の中だったらアクセス不可
  if (c <= threshold_) {
    accessable_map_.Set(point, false);
    return;
  }

  // 配置してみる
  Circle circle { point, kCharacterRadius };
  Vec2 pushbacked_pos = PushbackService::Exec(marching_squares_, circle);
  int x = static_cast<int>(std::round(pushbacked_pos.x));
  int y = static_cast<int>(std::round(pushbacked_pos.y));
  Point pushed_point { x, y };
  if (pushed_point != point) {
    accessable_map_.Set(point, false);
    return;
  }
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

  // 斜めを確認
  const bool ul = accessable_map_.Get(point.movedBy(-1, -1));
  const bool ur = accessable_map_.Get(point.movedBy(1, -1));
  const bool dl = accessable_map_.Get(point.movedBy(-1, 1));
  const bool dr = accessable_map_.Get(point.movedBy(1, 1));
  direction.up_left = ul && (direction.up || direction.left);
  direction.up_right = ur && (direction.up || direction.right);
  direction.down_left = dl && (direction.down || direction.left);
  direction.down_right = dr && (direction.down || direction.right);

  // 隣接ノードの逆方向も設定
  direction_map_.Set(point, direction);
  if (direction.up && accessable_map_.Get(point.movedBy(0, -1))) {
    auto temp = direction_map_.Get(point.movedBy(0, -1));
    temp.down = true;
    direction_map_.Set(point.movedBy(0, -1), temp);
  }
  if (direction.down && accessable_map_.Get(point.movedBy(0, 1))) {
    auto temp = direction_map_.Get(point.movedBy(0, 1));
    temp.up = true;
    direction_map_.Set(point.movedBy(0, 1), temp);
  }
  if (direction.left && accessable_map_.Get(point.movedBy(-1, 0))) {
    auto temp = direction_map_.Get(point.movedBy(-1, 0));
    temp.right = true;
    direction_map_.Set(point.movedBy(-1, 0), temp);
  }
  if (direction.right && accessable_map_.Get(point.movedBy(1, 0))) {
    auto temp = direction_map_.Get(point.movedBy(1, 0));
    temp.left = true;
    direction_map_.Set(point.movedBy(1, 0), temp);
  }
  if (direction.up_left && accessable_map_.Get(point.movedBy(-1, -1))) {
    auto temp = direction_map_.Get(point.movedBy(-1, -1));
    temp.down_right = true;
    direction_map_.Set(point.movedBy(-1, -1), temp);
  }
  if (direction.up_right && accessable_map_.Get(point.movedBy(1, -1))) {
    auto temp = direction_map_.Get(point.movedBy(1, -1));
    temp.down_left = true;
    direction_map_.Set(point.movedBy(1, -1), temp);
  }
  if (direction.down_left && accessable_map_.Get(point.movedBy(-1, 1))) {
    auto temp = direction_map_.Get(point.movedBy(-1, 1));
    temp.up_right = true;
    direction_map_.Set(point.movedBy(-1, 1), temp);
  }
  if (direction.down_right && accessable_map_.Get(point.movedBy(1, 1))) {
    auto temp = direction_map_.Get(point.movedBy(1, 1));
    temp.up_left = true;
    direction_map_.Set(point.movedBy(1, 1), temp);
  }

}

}
