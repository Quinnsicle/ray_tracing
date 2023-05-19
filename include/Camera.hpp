#ifndef _RAY_TRACING_LIB_CAMERA_HPP_
#define _RAY_TRACING_LIB_CAMERA_HPP_

#include "common.hpp"

class Camera {
 public:
  Camera(Point3 look_from, Point3 look_at, Vec3 up, double vertical_fov,
         double aspect_ratio, double aperture, double focus_distance) {
    auto theta = degrees_to_radians(vertical_fov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2.0 * h;
    auto viewport_width = aspect_ratio * viewport_height;

    w = unit_vector(look_from - look_at);
    u = unit_vector(cross(up, w));
    v = cross(w, u);

    origin = look_from;
    horizontal = focus_distance * viewport_width * u;
    vertical = focus_distance * viewport_height * v;
    top_left_corner =
        origin - horizontal / 2 + vertical / 2 - focus_distance * w;

    lens_radius = aperture / 2;
  }

  Ray get_ray(double s, double t) const {
    Vec3 rd = lens_radius * random_in_unit_circle();
    Vec3 offset = u * rd.x() + v * rd.y();

    return Ray(origin + offset, top_left_corner + s * horizontal -
                                    t * vertical - origin - offset);
  }

 private:
  Point3 origin;
  Point3 top_left_corner;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 u, v, w;
  double lens_radius;
};

#endif  // _RAY_TRACING_LIB_CAMERA_HPP_
