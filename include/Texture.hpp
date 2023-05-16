#ifndef _RAY_TRACING_LIB_TEXTURE_HPP_
#define _RAY_TRACING_LIB_TEXTURE_HPP_

#include "common.hpp"

class Texture {
 public:
  virtual Color value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
 public:
  SolidColor() {}
  SolidColor(Color c) : color_value(c) {}

  SolidColor(double red, double green, double blue)
      : SolidColor(Color(red, green, blue)) {}

  virtual Color value(double u, double v, const Vec3& p) const override {
    return color_value;
  }

 private:
  Color color_value;
};

class CheckerTexture : public Texture {
 public:
  CheckerTexture() {}

  CheckerTexture(shared_ptr<Texture> _even, shared_ptr<Texture> _odd)
      : even(_even), odd(_odd) {}

  CheckerTexture(Color c1, Color c2)
      : even(make_shared<SolidColor>(c1)), odd(make_shared<SolidColor>(c2)) {}

  virtual Color value(double u, double v, const Point3& p) const override {
    auto sines =
        std::sin(10 * p.x()) * std::sin(10 * p.y()) * std::sin(10 * p.z());
    return (sines < 0) ? odd->value(u, v, p) : even->value(u, v, p);
  }

 public:
  shared_ptr<Texture> odd;
  shared_ptr<Texture> even;
};

#endif
