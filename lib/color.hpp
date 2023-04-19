#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>

void write_color(std::ostream &out, color c) {
  // Write the translated [0,255] value of each color component
  out << static_cast<int>(255.999 * c.x()) << ' '
      << static_cast<int>(255.999 * c.y()) << ' '
      << static_cast<int>(255.999 * c.z()) << '\n';
}

#endif