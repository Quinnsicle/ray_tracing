#ifndef _RAY_TRACING_LIB_CAMERA_HPP_
#define _RAY_TRACING_LIB_CAMERA_HPP_

#include "common.hpp"

class camera {
 public:
  camera() {
    auto aspect_ratio = 16.0 / 9.0;
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    horizontal = vec3(viewport_width, 0, 0);
    vertical = vec3(0, viewport_height, 0);
    lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
  }

  ray get_ray(double u, double v) const {
    return ray(origin,
               lower_left_corner + u * horizontal + v * vertical - origin);
  }

 private:
  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
};

#endif  // _RAY_TRACING_LIB_CAMERA_HPP_