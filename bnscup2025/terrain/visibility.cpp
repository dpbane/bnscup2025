#include "stdafx.h"
#include "visibility.hpp"

namespace bnscup2025::terrain {

Visibility::Visibility(const Terrain& terrain) :
  terrain_(terrain) {
}

Array<Triangle> Visibility::CalcVisibilityTriangles(const Vec2& center, const SizeF& cell_size, const Vec2& eye_position, const Vec2& direction, double fov, double length) const {
  // 交点情報の定義
  struct IntersectInfo {
    double angle { 0 };
    Vec2 point_left { 0, 0 };
    Vec2 point_right { 0, 0 };
  };

  // 視点方向を基準とする角度を求めるラムダ
  auto AngleFromDirection = [&](const Vec2& point) -> double {
    const Vec2 dp = point - eye_position;
    double angle = dp.getAngle() - direction.getAngle();
    if (angle < -Math::Pi) {
      angle += Math::TwoPi;
    }
    else if (angle > Math::Pi) {
      angle -= Math::TwoPi;
    }
    return angle;
  };

  // Step 1: 壁となる線分群のうち、2つの端点が視野内にないものを除外する
  auto lines = terrain_.CreateVisibleWallLines(center, cell_size);  // 画面内に存在する線分
  for (auto itr = lines.begin(); itr != lines.end(); ) {
    const double angle1 = AngleFromDirection(itr->begin);
    const double angle2 = AngleFromDirection(itr->end);
    if (
      (angle1 > fov / 2.0 && angle2 > fov / 2.0) ||    // 両端点が正側にある
      (angle1 < -fov / 2.0 && angle2 < -fov / 2.0) ||  // 両端点が負側にある
      (fabs(angle1) > Math::HalfPi && fabs(angle2) > Math::HalfPi)  // 両端点が視点の背後にある
      ) {
      itr = lines.erase(itr);
    }
    else {
      ++itr;
    }
  }

  // Step 2: 各線分の端点の角度を記録し、ソート
  Array<double> angle_list { Arg::reserve = lines.size() * 2 + 2 };
  for (const auto& line : lines) {
    double angle1 = AngleFromDirection(line.begin);
    double angle2 = AngleFromDirection(line.end);
    if (angle1 >= -fov / 2.0 && angle1 <= fov / 2.0) angle_list.push_back(angle1);
    if (angle2 >= -fov / 2.0 && angle2 <= fov / 2.0) angle_list.push_back(angle2);
  }
  angle_list.push_back(-fov / 2.0);
  angle_list.push_back(fov / 2.0);
  angle_list.sort();

  // Step 3: 各角度に対してレイを飛ばし、最も近い交点を求める
  constexpr double epsilon = 1e-10;
  const Vec2 ray_base = direction * length;
  Array<IntersectInfo> intersect_infos { Arg::reserve = angle_list.size() };
  for (const double angle : angle_list) {
    const Vec2 ray_left_end = ray_base.rotated(angle - epsilon);
    const Vec2 ray_right_end = ray_base.rotated(angle + epsilon);
    Line ray_left { eye_position, eye_position + ray_left_end };
    Line ray_right { eye_position, eye_position + ray_right_end };
    double min_dist_left = std::numeric_limits<double>::infinity();
    double min_dist_right = std::numeric_limits<double>::infinity();

    IntersectInfo info {
      .angle = angle,
      .point_left = eye_position + ray_left_end,
      .point_right = eye_position + ray_right_end
    };
    for (const auto& test_line : lines) {
      // 左レイとの交点
      if (const auto p = ray_left.intersectsAt(test_line)) {
        const double dist = p->distanceFrom(eye_position);
        if (dist < min_dist_left) {
          min_dist_left = dist;
          info.point_left = *p;
        }
      }
      // 右レイとの交点
      if (const auto p = ray_right.intersectsAt(test_line)) {
        const double dist = p->distanceFrom(eye_position);
        if (dist < min_dist_right) {
          min_dist_right = dist;
          info.point_right = *p;
        }
      }
    }
    intersect_infos.push_back(info);
  }

  // Step 4: 交点情報から三角形群を生成
  Array<Triangle> triangles;
  for (int k = 0; k < (int)(intersect_infos.size()) - 1; ++k) {
    const auto& info1 = intersect_infos[k];
    const auto& info2 = intersect_infos[(k + 1)];
    triangles.emplace_back(
      eye_position,
      info1.point_right,
      info2.point_left
    );
  }

  return triangles;
}

}
