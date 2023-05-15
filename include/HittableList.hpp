#ifndef _RAY_TRACING_LIB_HITTABLE_LIST_HPP_
#define _RAY_TRACING_LIB_HITTABLE_LIST_HPP_

#include <memory>
#include <vector>

#include "Hittable.hpp"

using std::make_shared;
using std::shared_ptr;

// TODO: I think this is where I would implement a BVH (Bounding Volume
// Hierarchy)

class HittableList : public Hittable {
 public:
  HittableList() {}
  HittableList(shared_ptr<Hittable> object) { add(object); }

  void clear() { objects.clear(); }
  void add(shared_ptr<Hittable> object) { objects.push_back(object); }

  virtual bool hit(const Ray& r, double t_min, double t_max,
                   hit_record& rec) const override;

 public:
  std::vector<shared_ptr<Hittable>> objects;
};

bool HittableList::hit(const Ray& r, double t_min, double t_max,
                       hit_record& rec) const {
  hit_record hit_rec;  // is this necessary?
  bool hit_anything = false;
  auto closest_so_far = t_max;

  for (const auto& object : objects) {
    if (object->hit(r, t_min, closest_so_far, hit_rec)) {
      hit_anything = true;
      closest_so_far = hit_rec.t;
      rec = hit_rec;
    }
  }

  return hit_anything;
}

#endif  // _RAY_TRACING_LIB_HITTABLE_LIST_HPP_
