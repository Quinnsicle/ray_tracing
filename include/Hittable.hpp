#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "Ray.hpp"
#include "common.hpp"

class Material;

struct hit_record {
  Point3 p;
  Vec3 normal;
  shared_ptr<Material> mat_ptr;
  double t;
  double u;
  double v;
  bool front_face;

  /**
   * Sets face normal at geometry time instead of at material time.
   * The normal will always point towards the outside:    <-( )
   * */
  inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
 public:
  virtual bool hit(const Ray& r, double t_min, double t_max,
                   hit_record& rec) const = 0;
};

#endif
