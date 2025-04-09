#include <raylib.h>

#include <vector>

#include "../src/object.h"
#include "../src/vec2.h"
#include "demo.h"

struct Collision {
  Circle& a;
  Circle& b;
};

class Game : public Demo {
 public:
  std::vector<Circle> circles;
  Circle* selected;

  Game() {
    add_circle(screen.x * 0.25, screen.y / 2);
    add_circle(screen.x * 0.75, screen.y / 2);
  };

  void add_circle(float x, float y, float radius = 50.0f) {
    circles.emplace_back(vec2(x, y), vec2(0, 0), 5.0f, radius, RAYWHITE);
  }

  void update(float dt) override {
    // vec2 mouse = GetMousePosition();
    //
    // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //   selected = nullptr;
    //   for (auto& circle : circles) {
    //     if (CheckCollisionPointCircle(mouse, circle.pos, circle.radius)) {
    //       selected = &circle;
    //       break;
    //     }
    //   }
    // }
    //
    // if (selected != nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    //   selected->pos = mouse;
    // }
    //
    // if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    //   selected = nullptr;
    // }
    //
    // for (size_t i = 0; i < circles.size(); i++) {
    //   Circle& a = circles[i];
    //
    //   for (size_t j = i + 1; j < circles.size(); j++) {
    //     Circle& b = circles[j];
    //
    //     vec2 delta = b.pos - a.pos;
    //     float distance = delta.length();
    //
    //     // std::cout << delta << ' ' << distance << '\n';
    //
    //     if (distance <= a.radius + b.radius) {
    //       float overlap = (distance - a.radius - b.radius) * 0.5f;
    //       std::cout << "overlap = " << overlap << '\n';
    //       // vec2 norm = delta.norm();
    //       // a.pos -= norm * overlap;
    //       // b.pos += norm * overlap;
    //     }
    //   }
    // }
  }

  void draw() override {
    for (auto& circle : circles) {
      DrawCircleV(circle.pos, circle.radius, circle.color);
    }
  }
};

int main() {
  Game game;
  game.run();
}
