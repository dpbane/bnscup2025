#include "stdafx.h"
#include "pushback_service.hpp"

namespace bnscup2025::terrain {

Vec2 PushbackService::Exec(const MarchingSquares& marching_squares, const Circle& circle) {
  const double epsilon_sq = 1e-12;

  Vec2 current_position = circle.center;
  for (int count = 0; count < 10; ++count) {
    const Vec2 result = DoOneStep(marching_squares, Circle { current_position, circle.r });
    if (result.distanceFromSq(current_position) < epsilon_sq) return result;
    current_position = result;
  }
  return current_position;
}

Vec2 PushbackService::DoOneStep(const MarchingSquares& marching_squares, const Circle& circle) {
  const auto rect = circle.boundingRect();
  const int x_min = static_cast<int>(std::floor(rect.leftX()));
  const int x_max = static_cast<int>(std::ceil(rect.rightX()));
  const int y_min = static_cast<int>(std::floor(rect.topY()));
  const int y_max = static_cast<int>(std::ceil(rect.bottomY()));

  Circle current_circle = circle;

  for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
      const Point cell_pos { x, y };
      const auto lines = marching_squares.GetEdgeLines().Get(cell_pos);

      for (const auto& line_array : lines) {
        for (size_t i = 0; i < line_array.size() - 1; ++i) {
          const Line line { line_array[i], line_array[i + 1] };

          // Step 1: 円と線分が交差するかを確認し、領域の内外を判定する。
          // 線分は領域を反時計回りに囲むように配置されているので、外積で判定できる。
          if (not current_circle.intersects(line)) continue;
          const Vec2 line_dir = line.end - line.begin;
          const Vec2 to_circle_center = current_circle.center - line.begin;
          if (line_dir.cross(to_circle_center) < 0.0) continue;

          // Step 2: 円の中心を線分に投影したときの媒介変数tdを求める。
          // https://zenn.dev/boiledorange73/articles/0037-js-distance-pt-seg
          const auto& p1 = line.begin;
          const auto& p2 = line.end;
          const auto& p0 = current_circle.center;
          const double td = -(p2 - p1).dot(p1 - p0);

          // Step 3: 円の中心に最も近い線分上の点を求める。
          Vec2 nearest_point {};
          if (td <= 0.0) {
            nearest_point = p1;
          }
          else if (td >= (p2 - p1).lengthSq()) {
            nearest_point = p2;
          }
          else {
            const double t = td / (p2 - p1).lengthSq();
            nearest_point = p1 + (p2 - p1) * t;
          }

          // Step 4: 押し戻すためのベクトルを求める。
          const Vec2 pushback_vector = current_circle.center - nearest_point;
          const double pushback_distance = current_circle.r - pushback_vector.length();

          // Step 5: 押し戻した後の位置を返す。
          current_circle.center += pushback_vector.normalized() * pushback_distance;
        }
      }
    }
  }

  return current_circle.center;
}

}
