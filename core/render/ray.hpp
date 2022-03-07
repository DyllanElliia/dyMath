/*
 * @Author: DyllanElliia
 * @Date: 2022-03-01 14:50:58
 * @LastEditors: DyllanElliia
 * @LastEditTime: 2022-03-07 17:00:10
 * @Description:
 */
#pragma once
#include "randomFun.hpp"

namespace dym {
namespace rt {
class Ray {
 public:
  Ray() {}
  Ray(const Point3& origin, const Vector3& direction, const Real& time = 0.f)
      : orig(origin), dir(direction), tm(time) {}

  Point3 origin() const { return orig; }
  Vector3 direction() const { return dir; }
  Real time() const { return tm; }

  Point3 at(Real t) const { return orig + t * dir; }

 public:
  Point3 orig;  // original point
  Vector3 dir;  // direction vector
  Real tm;
};

class Material;
struct HitRecord {
  Point3 p;
  Vector3 normal;
  Real t;
  Real u, v;
  bool front_face;
  shared_ptr<Material> mat_ptr;

  inline void set_face_normal(const Ray& r, const Vector3& outward_normal) {
    front_face = vector::dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

}  // namespace rt
}  // namespace dym