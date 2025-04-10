#include <raylib.h>

#include <vector>

#include "../src/object.h"
#include "../src/vec2.h"
#include "demo.h"

struct VerletObject {
  vec2 pos;
  vec2 prev_pos;
  float mass;
  Color color;
  float radius = 25.0f;

  VerletObject(float x, float y, float mass, Color color)
      : pos(vec2(x, y)), prev_pos(vec2(x, y)), mass(mass), color(color) {}
};

struct Stick {
  vec2 start;
  vec2 end;
  float length;

  Stick(vec2 start, vec2 end, float length) : start(start), end(end), length(length) {};
};

class Game : public Demo {
 public:
  std::vector<Circle> circles;
  std::vector<VerletObject> objects;
  vec2 force = {100, 300};

  static Game euler() {
    Game game;
    float screen_width = game.screen.x;
    float screen_height = game.screen.y;
    game.add_circle(screen_width * 0.25, screen_height / 2, RED);
    game.add_circle(screen_width * 0.75, screen_height / 2, GREEN);
    game.add_circle(screen_width * 0.5, screen_height / 2, BLUE);
    return game;
  }

  static Game verlet() {
    Game game;
    float screen_width = game.screen.x;
    float screen_height = game.screen.y;
    game.add_object(screen_width * 0.25, screen_height / 2, RED);
    game.add_object(screen_width * 0.75, screen_height / 2, GREEN);
    return game;
  }

 private:
  Game() {};

  void add_circle(float x, float y, Color color, float radius = 25.0f) {
    circles.emplace_back(vec2(x, y), vec2(0, 50), 1.0f, radius, color);
  }

  void add_object(float x, float y, Color color) {
    objects.emplace_back(x, y, 1.0f, color);
  }

  void constrain(Circle& circle) {
    if (circle.pos.x - circle.radius < 0) {
      circle.pos.x = circle.radius;
      circle.vel.x *= -1;
    } else if (circle.pos.x + circle.radius > screen.x) {
      circle.pos.x = screen.x - circle.radius;
      circle.vel.x *= -1;
    } else if (circle.pos.y - circle.radius < 0) {
      circle.pos.y = 0;
      circle.vel.y *= -1;
    } else if (circle.pos.y + circle.radius > screen.y) {
      circle.pos.y = screen.y - circle.radius;
      circle.vel.y *= -1;
    }
  }

  void euler_method(float dt, Circle& circle) {
    vec2 acc = force / circle.mass;
    circle.pos += circle.vel * dt;
    circle.vel += acc * dt;
  }

  void semi_implicit_euler_method(float dt, Circle& circle) {
    vec2 acc = force / circle.mass;
    circle.vel += acc * dt;
    circle.pos += circle.vel * dt;
  }

  void verlet_integration(float dt, VerletObject& obj) {
    vec2 displacement = obj.pos - obj.prev_pos;

    // The current position becomes the old one
    obj.prev_pos = obj.pos;

    // Compute the acceleration using a=F/m
    vec2 acceleration = force / obj.mass;

    // Estimate the new position using Verlet Integration
    obj.pos += displacement + acceleration * (dt * dt);
  }

  void verlet_constrain(VerletObject& obj) {
    vec2 vel = obj.pos - obj.prev_pos;

    if (obj.pos.x < 0) {
      obj.pos.x = 0;
      obj.prev_pos.x = obj.pos.x + vel.x;
    } else if (obj.pos.x > screen.x) {
      obj.pos.x = screen.x;
      obj.prev_pos.x = obj.pos.x + vel.x;
    } else if (obj.pos.y < 0) {
      obj.pos.y = 0;
      obj.prev_pos.y = obj.pos.y + vel.y;
    } else if (obj.pos.y > screen.y) {
      obj.pos.y = screen.y - obj.radius;
      obj.prev_pos.y = obj.pos.y + vel.y;
    }
  }

  void update(float dt) override {
    for (auto& circle : circles) {
      // euler_method(dt, circle);
      semi_implicit_euler_method(dt, circle);
      constrain(circle);
    }
    for (auto& obj : objects) {
      verlet_integration(dt, obj);
      verlet_constrain(obj);
    }
  }

  void draw() override {
    for (auto& circle : circles) {
      DrawCircleV(circle.pos, circle.radius, circle.color);
    }

    for (auto& obj : objects) {
      DrawCircleV(obj.pos, obj.radius, obj.color);
    }

    DrawLineV({0, screen.y / 2}, {screen.x, screen.y / 2}, LIGHTGRAY);
    DrawLineV({screen.x / 2, 0}, {screen.x / 2, screen.y}, LIGHTGRAY);
  }
};

int main() {
  // Game game = Game::euler();
  Game game = Game::verlet();
  game.run();
}
