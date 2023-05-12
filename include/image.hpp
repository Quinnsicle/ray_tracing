#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <fstream>
#include <iostream>
#include <vector>

#include "vec3.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void check_file(std::string file_name) {
  std::fstream file;
  file.open(file_name, std::fstream::in | std::fstream::app);

  // check that file exists, if not then create it
  if (!file) {
    std::cout << "Cannot open " << file_name
              << ", file does not exist. Creating new file..." << std::endl;

    file.open(file_name,
              std::fstream::in | std::fstream::out | std::fstream::trunc);
    file << "\n";
    file.close();
  }
}

class Img {
 public:
  virtual void write(std::string file_name, const std::vector<Color>& pixels,
                     const int& samples) const = 0;

 private:
};

class Jpg : public Img {
 public:
  Jpg(const int w, const int h) : width_(w), height_(h) {}

  virtual void write(std::string file_name, const std::vector<Color>& pixels,
                     const int& samples) const override {
    file_name.append(".Jpg");

    check_file(file_name);

    unsigned char data[width_ * height_ * channel_nums];
    int index = 0;

    for (Color p : pixels) {
      auto r = p.x();
      auto g = p.y();
      auto b = p.z();

      // Divide the Color by the number of samples and gamma-correct for
      // gamma=2.0.
      auto scale = 1.0 / samples;
      r = sqrt(scale * r);
      g = sqrt(scale * g);
      b = sqrt(scale * b);

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

class ppm : public Img {
 public:
  ppm(int w, int h) : width_(w), height_(h) {}

  virtual void write(std::string file_name, const std::vector<Color>& pixels,
                     const int& samples) const override {
    file_name.append(".ppm");

    std::ofstream ppm_file{file_name};
    ppm_file << "P3\n" << width_ << ' ' << height_ << "\n255\n";

    for (Color p : pixels) {
      auto r = p.x();
      auto g = p.y();
      auto b = p.z();

      // Divide the Color by the number of samples and gamma-correct for
      // gamma=2.0.
      auto scale = 1.0 / samples;
      r = sqrt(scale * r);
      g = sqrt(scale * g);
      b = sqrt(scale * b);

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
