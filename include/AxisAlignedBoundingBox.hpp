#ifndef _RAY_TRACING_LIB_AXIS_ALIGNED_BOUNDING_BOX_HPP_
#define _RAY_TRACING_LIB_AXIS_ALIGNED_BOUNDING_BOX_HPP_

#include "common.hpp"

class AxisAlignedBoundingBox {
 public:
  AxisAlignedBoundingBox() {}
  AxisAlignedBoundingBox(const Point3& a, const Point3& b) {
    minimum = a;
    maximum = b;
  }

  Point3 min() const { return minimum; }
  Point3 max() const { return maximum; }

  bool hit(const Ray& r, double t_min, double t_max) const {
    // For each axis, calculate the intersection of the ray and the min/max axis
    // boundaries. For a hit, t0, which represents the min intersection should
    // be greater than t1, which is the max intersection.
    for (int a = 0; a < 3; a++) {
      double inverted_direction = 1.0f / r.direction()[a];
      double intersection_1 = (minimum[a] - r.origin()[a]) * inverted_direction;
      double intersection_2 = (maximum[a] - r.origin()[a]) * inverted_direction;
      double t0 = std::fmin(intersection_1, intersection_2);
      double t1 = std::fmax(intersection_1, intersection_2);
      t_min = std::fmax(t0, t_min);
      t_max = std::fmin(t1, t_max);
      if (t_max <= t_min) {
        return false;
      }
    }
    return true;
  }

  Point3 minimum;
  Point3 maximum;
};

AxisAlignedBoundingBox surrounding_box(AxisAlignedBoundingBox box0,
                                       AxisAlignedBoundingBox box1) {
  Point3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));

  Point3 big(fmin(box0.max().x(), box1.max().x()),
             fmin(box0.max().y(), box1.max().y()),
             fmin(box0.max().z(), box1.max().z()));
  return AxisAlignedBoundingBox(small, big);
}

#endif
