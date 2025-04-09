#include "vec2.h"

struct Object {
  vec2 pos;
  vec2 vel;
  float mass;

  Object(vec2 pos, vec2 vel, float mass) : pos(pos), vel(vel), mass(mass) {}
};

struct Circle : Object {
  float radius;
  Color color;
  std::vector<vec2> trail;
  const size_t max_trail_length = 30;

  Circle(vec2 pos, vec2 vel, float mass, float radius, Color color)
      : Object(pos, vel, mass), radius(radius), color(color) {}
};
