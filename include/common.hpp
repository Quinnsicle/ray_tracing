#ifndef __RAY_TRACING_LIB_COMMON_HPP_
#define __RAY_TRACING_LIB_COMMON_HPP_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.145926535897932385;
const double inverse_sin45 = 1 / 0.7071;

// Utility Functions

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

/// @brief Returns a random real in [0,1).
inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937_64 generator;
  return distribution(generator);
}

/// @brief Returns a random real in [min,max).
inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}

/// @brief Returns a random integer in [min,max].
inline int random_int(int min, int max) {
  return static_cast<int>(random_double(min, max + 1));
}

inline double clamp(double x, double min, double max) {
  if (x < min) {
    return min;
  }
  if (x > max) {
    return max;
  }
  return x;
}

// Common Headers

#include "Ray.hpp"
#include "Vec3.hpp"

#endif  // _RAY_TRACING_LIB_COMMON_HPP_
