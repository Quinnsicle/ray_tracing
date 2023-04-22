#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "vec3.hpp"

class img {
 public:
  virtual void write(std::string file_name, const std::vector<color>& pixels,
                     const int& samples) const = 0;

 private:
};

class jpg : public img {
 public:
  jpg(const int w, const int h) : width_(w), height_(h) {}

  virtual void write(std::string file_name, const std::vector<color>& pixels,
                     const int& samples) const override {
    file_name.append(".jpg");

    unsigned char data[width_ * height_ * channel_nums];
    int index = 0;

    for (color p : pixels) {
      auto r = p.x();
      auto g = p.y();
      auto b = p.z();

      // Divide the color by the number of samples.
      auto scale = 1.0 / samples;
      r *= scale;
      g *= scale;
      b *= scale;

      data[index++] = 255.999 * clamp(r, 0.0, 0.999);
      data[index++] = 255.999 * clamp(g, 0.0, 0.999);
      data[index++] = 255.999 * clamp(b, 0.0, 0.999);
    }

    stbi_write_jpg(file_name.c_str(), width_, height_, 3, data, quality);
  }

  int quality = 100;
  int channel_nums = 3;

 private:
  const int width_;
  const int height_;
};

class ppm : public img {
 public:
  ppm(int w, int h) : width_(w), height_(h) {}

  virtual void write(std::string file_name, const std::vector<color>& pixels,
                     const int& samples) const override {
    file_name.append(".ppm");

    std::ofstream ppm_file{file_name};
    ppm_file << "P3\n" << width_ << ' ' << height_ << "\n255\n";

    for (color p : pixels) {
      auto r = p.x();
      auto g = p.y();
      auto b = p.z();

      // Divide the color by the number of samples.
      auto scale = 1.0 / samples;
      r *= scale;
      g *= scale;
      b *= scale;

      ppm_file << static_cast<int>(255.999 * clamp(r, 0.0, 0.999)) << ' '
               << static_cast<int>(255.999 * clamp(g, 0.0, 0.999)) << ' '
               << static_cast<int>(255.999 * clamp(b, 0.0, 0.999)) << '\n';
    }
  }

 private:
  int width_;
  int height_;
};

#endif
