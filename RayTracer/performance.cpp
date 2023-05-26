#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Camera.hpp"
#include "HittableList.hpp"
#include "Image.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Texture.hpp"
#include "Vec3.hpp"
#include "color.hpp"
#include "common.hpp"

Color ray_color(const Ray& r, const Hittable& world, int depth) {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) {
    return Color(0, 0, 0);
  }

  if (world.hit(r, 0.001, infinity, rec)) {
    Ray scattered;
    Color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, depth - 1);
    }
    return Color(0, 0, 0);
  }
  Vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

HittableList random_scene() {
  HittableList world;

  auto checker =
      make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
  auto ground_material = make_shared<Lambertian>(checker);
  world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_material = random_double();
      Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> sphere_material;

        if (choose_material < 0.8) {
          // diffuse
          auto albedo = Color::random() * Color::random();
          sphere_material = make_shared<Lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_material < 0.95) {
          // Metal
          auto albedo = Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));

        } else {
          // glass
          sphere_material = make_shared<Dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<Dielectric>(1.5);
  world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  return world;
}

Color sample_pixel(const int& x, const int& y, const int& width,
                   const int& height, const Camera& camera,
                   const HittableList& world, const int& depth) {
  auto u = (x + random_double()) / (width - 1);
  auto v = (y + random_double()) / (height - 1);
  Ray r = camera.get_ray(u, v);
  return ray_color(r, world, depth);
}

int main() {
  // Image
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 1280;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const double total_pixels = image_height * image_width;
  Jpg jpg_image(image_width, image_height);
  const int samples_per_pixel = 20;
  const int max_depth = 20;

  // World
  HittableList world = random_scene();

  // Camera
  Point3 look_from(13, 2, 3);
  Point3 look_at(0, 0, 0);
  Vec3 up(0, 1, 0);
  auto distance_to_focus = (look_from - look_at).length();
  auto aperture = 0.1;

  Camera cam(look_from, look_at, up, 20, aspect_ratio, aperture,
             distance_to_focus);

  // Render
  const int num_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(num_threads);
  std::mutex pixels_mutex;

  std::vector<Color> pixels1(total_pixels);

  // track multithreaded performance
  auto start_time_mt = std::chrono::high_resolution_clock::now();

  for (int t = 0; t < num_threads; ++t) {
    threads[t] = std::thread([&, t]() {
      for (int j = t; j < image_height; j += num_threads) {
        for (int i = 0; i < image_width; ++i) {
          Color pixel_color(0, 0, 0);
          for (int s = 0; s < samples_per_pixel; ++s) {
            pixel_color += sample_pixel(i, j, image_width, image_height, cam,
                                        world, max_depth);
          }

          std::lock_guard<std::mutex> guard(pixels_mutex);
          pixels1[i + (j * image_width)] = (pixel_color / samples_per_pixel);
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }
  auto end_time_mt = std::chrono::high_resolution_clock::now();

  // track single threaded performance

  std::vector<Color> pixels2(total_pixels);

  auto start_time_st = std::chrono::high_resolution_clock::now();

  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      Color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        pixel_color += sample_pixel(i, j, image_width, image_height, cam, world,
                                    max_depth);
      }

      std::lock_guard<std::mutex> guard(pixels_mutex);
      pixels2[i + (j * image_width)] = (pixel_color / samples_per_pixel);
    }
  }
  auto end_time_st = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> mt_time =
      end_time_mt - start_time_mt;
  std::chrono::duration<double, std::milli> st_time =
      end_time_st - start_time_st;

  std::cerr << "\nDone.\n";
  std::cerr << "\n\nMulti-threaded time: " << mt_time.count() << '\n';
  std::cerr << "\nSingle-threaded time: " << st_time.count() << '\n';

  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
  std::cerr << "\n\nSystem: " << hostname << '\n';
  std::cerr << "Number of threads: " << num_threads << '\n';
  std::cerr << "Total Improvement: " << st_time.count() / mt_time.count()
            << " times faster!\n";

  return 0;
}
