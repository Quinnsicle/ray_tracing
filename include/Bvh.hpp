#ifndef _RAY_TRACING_LIB_BVH_HPP_
#define _RAY_TRACING_LIB_BVH_HPP_

#include <algorithm>

#include "Hittable.hpp"
#include "HittableList.hpp"
#include "common.hpp"

inline bool box_compare(const shared_ptr<Hittable> a,
                        const shared_ptr<Hittable> b, int axis) {
  AxisAlignedBoundingBox box_a;
  AxisAlignedBoundingBox box_b;

  if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) {
    std::cerr << "No bounding box in bvh_node constructor.\n";
  }

  return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
  return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
  return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
  return box_compare(a, b, 2);
}

class BvhNode : public Hittable {
 public:
  BvhNode();

  BvhNode(const HittableList& list)
      : BvhNode(list.objects, 0, list.objects.size()) {}

  BvhNode(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start,
          size_t end);

  virtual bool hit(const Ray& r, double t_min, double t_max,
                   HitRecord& rec) const override;

  virtual bool bounding_box(AxisAlignedBoundingBox& output_box) const override;

 public:
  shared_ptr<Hittable> left;
  shared_ptr<Hittable> right;
  AxisAlignedBoundingBox box;
};

BvhNode::BvhNode(const std::vector<shared_ptr<Hittable>>& src_objects,
                 size_t start, size_t end) {
  auto objects = src_objects;

  int axis = random_int(0, 2);
  auto comparator = (axis == 0)   ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

  size_t object_span = end - start;

  if (object_span == 1) {
    left = objects[start];
    right = objects[start];
  } else if (object_span == 2) {
    if (comparator(objects[start], objects[start + 1])) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      left = objects[start + 1];
      right = objects[start];
    }
  } else {
    std::sort(objects.begin() + start, objects.begin() + end, comparator);

    auto mid = start + object_span / 2;
    left = make_shared<BvhNode>(objects, start, mid);
    right = make_shared<BvhNode>(objects, mid, end);
  }

  AxisAlignedBoundingBox box_left, box_right;

  if (!left->bounding_box(box_left) || !right->bounding_box(box_right)) {
    std::cerr << "No bounding box in BvhNode constructor.\n";
  }

  box = surrounding_box(box_left, box_right);
}

bool BvhNode::hit(const Ray& r, double t_min, double t_max,
                  HitRecord& rec) const {
  if (!box.hit(r, t_min, t_max)) {
    return false;
  }

  bool hit_left = left->hit(r, t_min, t_max, rec);
  bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

  return hit_left || hit_right;
}

bool BvhNode::bounding_box(AxisAlignedBoundingBox& output_box) const {
  output_box = box;
  return true;
}

#endif
