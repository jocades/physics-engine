#pragma once

#include <vector>

#include "vec2.h"

namespace rng {

// Initializes the random engine (seeded with `std::random_device`)
void init();

// Generate a `random integer` in the given range [min, max]
int randint(int min, int max);

// Generate a `random float` in the given range [min, max]
float uniform(float min, float max);

// Generate a random `Vector2` with components in given ranges
vec2 randvec2(float min_x, float max_x, float min_y, float max_y);

// Generate a `random boolean` value
bool choice();

// Select a `random element` from a vector
template <typename T>
const T& choice(const std::vector<T>& v) {
  return v[randint(0, v.size() - 1)];
}

}  // namespace rng
