#include <raylib.h>

#include <vector>

#include "../src/object.h"
#include "../src/vec2.h"
#include "demo.h"

struct Collision {
  Circle* a;
  Circle* b;
  float distance;
};

class Game : public Demo {
 public:
  std::vector<Circle> circles;
  std::vector<Collision> collisions;
  vec2 mouse;
  Circle* selected = nullptr;
  std::vector<Color> colors = {GREEN, BLUE, PINK, ORANGE};

  Game() {
    int color_index = 0;
    for (int i = 0; i < 8; i++) {
      add_circle(
        GetRandomValue(25, screen.x - 25),
        GetRandomValue(25, screen.y - 25),
        colors[color_index],
        GetRandomValue(10, 50)
      );
      color_index = (color_index + 1) % colors.size();
    }
  };

  void add_circle(float x, float y, Color color = RAYWHITE, float radius = 50.0f) {
    float mass = 10.0f * radius;
    circles.emplace_back(vec2(x, y), vec2(0, 0), mass, radius, color);
  }

  void update(float dt) override {
    mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      selected = nullptr;
      for (auto& circle : circles) {
        if (CheckCollisionPointCircle(mouse, circle.pos, circle.radius)) {
          selected = &circle;
          break;
        }
      }
    }

    if (selected != nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      selected->pos = mouse;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      selected = nullptr;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
      if (selected != nullptr) {
        selected->vel = (selected->pos - mouse) * 5.0f;
      }
      selected = nullptr;
    }

    for (auto& circle : circles) {
      vec2 acc = -circle.vel * 0.8f;  // simulate drag;
      circle.vel += acc * dt;
      circle.pos += circle.vel * dt;

      if (circle.pos.x < 0) circle.pos.x += screen.x;
      if (circle.pos.x > screen.x) circle.pos.x -= screen.x;
      if (circle.pos.y < 0) circle.pos.y += screen.y;
      if (circle.pos.y > screen.y) circle.pos.y -= screen.y;

      if (abs(circle.vel.x * circle.vel.x + circle.vel.y * circle.vel.y) < 0.01f) {
        circle.vel = {0, 0};
      }
    }

    collisions.clear();
    for (size_t i = 0; i < circles.size(); i++) {
      Circle& a = circles[i];

      for (size_t j = i + 1; j < circles.size(); j++) {
        Circle& b = circles[j];

        vec2 delta = b.pos - a.pos;
        float distance = delta.length();

        if (distance <= a.radius + b.radius) {
          collisions.emplace_back(&a, &b, distance);
          float overlap = abs(distance - a.radius - b.radius) * 0.5f;
          vec2 norm = delta.norm();
          a.pos -= norm * overlap;
          b.pos += norm * overlap;
        }
      }
    }

    for (const auto& [a, b, distance] : collisions) {
      vec2 normal = (a->pos - b->pos).norm();

      // Tangent
      vec2 tangent = {-normal.y, normal.x};

      // Dot product tangent
      float dptan_a = a->vel.x * tangent.x + a->vel.y * tangent.y;
      float dptan_b = b->vel.x * tangent.x + b->vel.y * tangent.y;

      // Dot product normal
      float dpnorm_a = a->vel.x * normal.x + a->vel.y * normal.y;
      float dpnorm_b = b->vel.x * normal.x + b->vel.y * normal.y;

      // Conservation of momentum in 1D
      float momentum_a =
        (dpnorm_a * (a->mass - b->mass) + 2.0f * b->mass * dpnorm_b) / (a->mass + b->mass);
      float momentum_b =
        (dpnorm_b * (b->mass - a->mass) + 2.0f * a->mass * dpnorm_a) / (a->mass + b->mass);

      a->vel = tangent * dptan_a + normal * momentum_a;
      b->vel = tangent * dptan_b + normal * momentum_b;
    }
  }

  void draw() override {
    for (const auto& circle : circles) {
      DrawCircleV(circle.pos, circle.radius, circle.color);
    }

    for (const auto& [a, b, _] : collisions) {
      DrawLineV(a->pos, b->pos, RED);
    }

    if (selected != nullptr) {
      DrawLineV(selected->pos, mouse, MAGENTA);
    }
  }
};

int main() {
  Game game;
  game.run();
}
