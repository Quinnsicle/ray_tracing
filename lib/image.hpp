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
  image(int w, int h, int channels)  // put channels as a setter/getter instead
                                     // or just default it
      : width_(w), height_(h), channel_nums_(channels){};

  // save will append the file extension to the file_name
  void save(std::string file_name, file_format ff, std::vector<color> pixels,
            int quality = 100) {
    if (ff == file_format::jpg) {
      file_name.append(".jpg");

      unsigned char data[width_ * height_ * channel_nums_];
      int index = 0;

      for (color c : pixels) {
        data[index++] = 255.999 * c.x();
        data[index++] = 255.999 * c.y();
        data[index++] = 255.999 * c.z();
      }

      stbi_write_jpg(file_name.c_str(), width_, height_, 3, data, quality);
      return;
    }
    if (ff == file_format::ppm) {
      file_name.append(".ppm");

      std::ofstream ppm_file{file_name};
      ppm_file << "P3\n" << width_ << ' ' << height_ << "\n255\n";

      for (color c : pixels) {
        ppm_file << static_cast<int>(255.999 * c.x()) << ' '
                 << static_cast<int>(255.999 * c.y()) << ' '
                 << static_cast<int>(255.999 * c.z()) << '\n';
      }
    }
  };

 private:
  int width_;
  int height_;
  int channel_nums_;
};

#endif
