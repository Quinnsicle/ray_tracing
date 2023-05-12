#ifndef _RAY_TRACING_LIB_MATERIAL_HPP_
#define _RAY_TRACING_LIB_MATERIAL_HPP_

#include "common.hpp"
#include "hittable.hpp"

class material {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
 public:
  lambertian(const color& a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color& attenuation, ray& scattered) const override {
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero()) {
      scatter_direction = rec.normal;
    }

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

  color albedo;
};

class metal : public material {
 public:
  metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color& attenuation, ray& scattered) const override {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

 public:
  color albedo;
  double fuzz;
};

class dielectric : public material {
 public:
  dielectric(double index_of_refraction)
      : refraction_index(index_of_refraction) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       color& attenuation, ray& scattered) const override {
    attenuation = color(1.0, 1.0, 1.0);
    double refraction_ratio =
        rec.front_face ? (1.0 / refraction_index) : refraction_index;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction =
        (cannot_refract ||
         reflectance(cos_theta, refraction_ratio) > random_double())
            ? reflect(unit_direction, rec.normal)
            : refract(unit_direction, rec.normal, refraction_ratio);

    scattered = ray(rec.p, direction);
    return true;
  }

 public:
  double refraction_index;

 private:
  // Approximates reflectance using a method by Christophe Schlick
  static double reflectance(double cosine, double ref_index) {
    auto r0 = (1 - ref_index) / (1 + ref_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

#endif