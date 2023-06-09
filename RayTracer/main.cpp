#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Bvh.hpp"
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

#define PB_STR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PB_WIDTH 60

void print_progress(double percentage) {
  int display_percentage = (int)(percentage * 100);
  int lpad = (int)(percentage * PB_WIDTH);
  int rpad = PB_WIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", display_percentage, lpad, PB_STR, rpad, "");
  fflush(stdout);
}

Color ray_color(const Ray& r, const Color& background, const Hittable& world,
                int depth) {
  HitRecord rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0) {
    return Color(0, 0, 0);
  }

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, 0.001, infinity, rec)) {
    return background;
  }

  Ray scattered;
  Color attenuation;
  Color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
    return emitted;
  }
  return emitted +
         attenuation * ray_color(scattered, background, world, depth - 1);
}

Color sample_pixel(const int& x, const int& y, const int& width,
                   const int& height, const Camera& camera,
                   const Color& background, const Hittable& world,
                   const int& depth) {
  auto u = (x + random_double()) / (width - 1);
  auto v = (y + random_double()) / (height - 1);
  Ray r = camera.get_ray(u, v);
  return ray_color(r, background, world, depth);
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
        } else if (choose_material < 0.98) {
          // Light
          sphere_material = make_shared<DiffuseLight>(Color::random());
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

  auto sunlight = make_shared<DiffuseLight>(Color(10, 9, 8));
  world.add(make_shared<Sphere>(Point3(80, 300, 300), 100.0, sunlight));

  return HittableList(make_shared<BvhNode>(world));
}

HittableList solar_scene() {
  HittableList objects;

  auto sun_material = make_shared<DiffuseLight>(Color(5, 1, 1));
  objects.add(make_shared<Sphere>(Point3(0, 0, 0), 432.690, sun_material));

  auto mercury_material = make_shared<Lambertian>(Color(120, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(40'194, 0, 0), 1.516, mercury_material));

  auto venus_material = make_shared<Lambertian>(Color(230, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(67'077, 0, 0), 3.7604, venus_material));

  auto earth_material = make_shared<Lambertian>(Color(1, 1, 253));
  objects.add(
      make_shared<Sphere>(Point3(92'960, 0, 0), 3.9588, earth_material));

  auto mars_material = make_shared<Lambertian>(Color(253, 1, 1));
  objects.add(
      make_shared<Sphere>(Point3(155'780, 0, 0), 2.1061, mars_material));

  auto jupiter_material = make_shared<Lambertian>(Color(10, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(460'640, 0, 0), 43.441, jupiter_material));

  auto saturn_material = make_shared<Lambertian>(Color(10, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(909'600, 0, 0), 36.184, saturn_material));

  auto uranus_material = make_shared<Lambertian>(Color(10, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(1'825'700, 0, 0), 15.759, uranus_material));

  auto neptune_material = make_shared<Lambertian>(Color(10, 253, 10));
  objects.add(
      make_shared<Sphere>(Point3(2'779'500, 0, 0), 15.299, neptune_material));

  return HittableList(make_shared<BvhNode>(objects));
}

int main() {
  // Image
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 1280;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const double total_pixels = image_height * image_width;
  Jpg jpg_image(image_width, image_height);
  std::vector<Color> pixels(total_pixels);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  // World
  HittableList scene;

  Point3 look_from;
  Point3 look_at;
  auto vfov = 40.0;
  auto aperture = 0.0;
  std::string scene_name;

  switch (2) {
    case 1:
      scene_name = "random_spheres";
      scene = random_scene();
      look_from = Point3(13, 2, 3);
      look_at = Point3(0, 0, 0);
      aperture = 0.1;
      break;
    case 2:
      scene_name = "solar_system";
      scene = solar_scene();
      look_from = Point3(93'964, 0, 8);
      look_at = Point3(0, 0, -500);
      aperture = 0.1;
      break;
  }

  Color background = Color(0, 0, 0);

  // Camera
  Vec3 up(0, 1, 0);
  auto distance_to_focus = (look_from - look_at).length();

  Camera cam(look_from, look_at, up, vfov, aspect_ratio, aperture,
             distance_to_focus);

  // Render
  const int num_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(num_threads);
  std::mutex pixels_mutex;
  int progress_count = 0;

  for (int t = 0; t < num_threads; ++t) {
    threads[t] = std::thread([&, t]() {
      for (int j = t; j < image_height; j += num_threads) {
        for (int i = 0; i < image_width; ++i) {
          Color pixel_color(0, 0, 0);
          for (int s = 0; s < samples_per_pixel; ++s) {
            pixel_color += sample_pixel(i, j, image_width, image_height, cam,
                                        background, scene, max_depth);
          }

          std::lock_guard<std::mutex> guard(pixels_mutex);
          pixels[i + (j * image_width)] = (pixel_color / samples_per_pixel);
          progress_count++;
        }
        print_progress(progress_count / total_pixels);
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  jpg_image.write("img/" + scene_name, pixels);

  return 0;
}
