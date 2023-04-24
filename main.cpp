#include <iostream>
#include <vector>

#include "lib/camera.hpp"
#include "lib/color.hpp"
#include "lib/common.hpp"
#include "lib/hittable_list.hpp"
#include "lib/image.hpp"
#include "lib/ray.hpp"
#include "lib/sphere.hpp"
#include "lib/vec3.hpp"

color ray_color(const ray& r, const hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) {
    return color(0, 0, 0);
  }

  if (world.hit(r, 0, infinity, rec)) {
    point3 target = rec.p + rec.normal + random_unit_vector();
    return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {
  // Image

  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  jpg jpg_image(image_width, image_height);
  std::vector<color> pixels;
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  // World
  hittable_list world;
  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

  // Camera
  camera cam;

  // Render

  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanLines remaining: " << j << std::flush;
    for (int i = 0; i < image_width; ++i) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }

      pixels.push_back(pixel_color);
    }
  }

  jpg_image.write("img/jpg_image", pixels, samples_per_pixel);

  std::cerr << "\nDone.\n";
  return 0;
}