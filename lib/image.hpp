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

enum file_format {
  jpg,
  ppm,
};

class image {
 public:
  image(int w, int h, file_format ff,
        int channels)  // put channels as a setter/getter instead
                                     // or just default it
      : width_(w), height_(h), format_(ff), channel_nums_(channels){};

  void write_color(){};

  // save will append the file extension to the file_name
  void save(std::string file_name, std::vector<color> pixels,
            int samples_per_pixel, int quality = 100) {
    if (format_ == file_format::jpg) {
      file_name.append(".jpg");

      unsigned char data[width_ * height_ * channel_nums_];
      int index = 0;

      for (color p : pixels) {
        auto r = p.x();
        auto g = p.y();
        auto b = p.z();

        // Divide the color by the number of samples.
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;

        data[index++] = 256 * clamp(r, 0.0, 0.999);
        data[index++] = 256 * clamp(g, 0.0, 0.999);
        data[index++] = 256 * clamp(b, 0.0, 0.999);
      }

      stbi_write_jpg(file_name.c_str(), width_, height_, 3, data, quality);
      return;
    }
    if (format_ == file_format::ppm) {
      file_name.append(".ppm");

      std::ofstream ppm_file{file_name};
      ppm_file << "P3\n" << width_ << ' ' << height_ << "\n255\n";

      for (color p : pixels) {
        auto r = p.x();
        auto g = p.y();
        auto b = p.z();

        // Divide the color by the number of samples.
        auto scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;

        // Write the translated [0,255] value of each color component.
        ppm_file << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                 << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                 << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
      }
    }
  };

 private:
  int width_;
  int height_;
  file_format format_;
  int channel_nums_;
};

#endif
