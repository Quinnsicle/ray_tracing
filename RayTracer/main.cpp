#include <iostream>
#include <vector>

#include "camera.hpp"
#include "color.hpp"
#include "common.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "vec3.hpp"

color ray_color(const ray& r, const hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) {
    return color(0, 0, 0);
  }

  if (world.hit(r, 0.001, infinity, rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, depth - 1);
    }
    return color(0, 0, 0);
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
  auto R = std::cos(pi / 4);
  hittable_list world;

  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
  auto material_left = make_shared<dielectric>(1.5);
  auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

  world.add(
      make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45,
  // material_left));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

  // Camera
  point3 look_from(3, 3, 2);
  point3 look_at(0, 0, -1);
  vec3 up(0, 1, 0);
  auto distance_to_focus = (look_from - look_at).length();
  auto aperture = 2.0;

  camera cam(look_from, look_at, up, 20, aspect_ratio, aperture,
             distance_to_focus);

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
