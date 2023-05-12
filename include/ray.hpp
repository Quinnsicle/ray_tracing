#ifndef RAY_HPP
#define RAY_HPP

#include "vec3.hpp"

class ray {
 public:
  ray() {}
  ray(const Point3& origin, const Vec3& direction)
      : orig(origin), dir(direction) {}

  Point3 origin() const { return orig; }
  Vec3 direction() const { return dir; }

  Point3 at(double t) const { return orig + t * dir; }

 public:
  Point3 orig;
  Vec3 dir;
};

#endif
