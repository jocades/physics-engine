#include <raylib.h>

#include <memory>
#include <vector>

#include "object.h"
#include "rng.h"
#include "vec2.h"

const vec2 screen = {1280, 720};
const Color BACKGROUND = GetColor(0x282828ff);

const float G = 1000.0f;         // px/s^2
const float RESTITUTION = 0.7f;  // Bounciness (0 = no bounce, 1 = perfect bounce)
const float VELOCITY_VECTOR_SCALE = 0.2f;
const float ATTRACT_FORCE = 150.0f;

bool debug = false;

void resolve_collision(Circle& a, Circle& b) {
  vec2 delta = b.pos - a.pos;
  float distance = delta.length();
  float min_dist = a.radius + b.radius;

  if (distance < min_dist && distance > 0.0f) {
    vec2 norm = delta.norm();
    vec2 relative_vel = b.vel - a.vel;
    float vel_along_normal = relative_vel.dot(norm);
    if (vel_along_normal > 0) return;

    float e = RESTITUTION;
    float j = -(1 + e) * vel_along_normal;
    j /= (1 / a.mass) + (1 / b.mass);

    vec2 impulse = norm * j;
    a.vel -= impulse * (1 / a.mass);
    b.vel += impulse * (1 / b.mass);

    float percent = 0.8f;
    float correction_mag = (min_dist / distance) / ((1 / a.mass) + (1 / b.mass)) * percent;
    vec2 correction = norm * correction_mag;
    a.pos -= correction * (1 / a.mass);
    b.pos += correction * (1 / b.mass);
  }
}

void draw_grid(float spacing) {
  for (int x = 0; x < screen.x / spacing; x++) {
    DrawLine(x * spacing, 0, x * spacing, screen.y, Fade(LIGHTGRAY, 0.25));
  }
  for (int y = 0; y < screen.y / spacing; y++) {
    DrawLine(0, y * spacing, screen.x, y * spacing, Fade(LIGHTGRAY, 0.25));
  }
}

struct Inner {
  vec2 pos = {screen.x / 2, screen.y / 2};
  float radius = 350;
};

int main() {
  InitWindow(screen.x, screen.y, "Physics Engine");
  SetTargetFPS(144);
  rng::init();

  const size_t objects_count = 2000;
  std::vector<Circle> objects;
  objects.reserve(objects_count);
  for (size_t i = 0; i < objects_count; i++) {
    objects.emplace_back(
      rng::randvec2(20, screen.x - 20, 20, screen.y - 20),
      rng::randvec2(-100, 100, -100, 100),
      2.f,
      5.0f,
      RAYWHITE
    );
  }

  // std::vector<Circle> objects = {
  //   Circle({300, 100}, {100, 50}, 2.0f, 30, RED),
  //   Circle({500, 150}, {-80, 0}, 4.0f, 40, BLUE),
  //   Circle({700, 200}, {-50, -30}, 3.0f, 35, GREEN),
  // };

  // std::vector<Circle> objects = {
  //   Circle({screen.x / 2 - 150, 100}, {0, 0}, 2.0f, 20, BLUE),
  //   Circle({screen.x / 2 - 200, 100}, {0, 0}, 2.0f, 20, RED),
  // };

  Inner inner;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    vec2 mouse_pos = GetMousePosition();
    if (IsKeyPressed(KEY_D)) debug = !debug;

    for (auto& ball : objects) {
      if (debug) {
        ball.trail.push_back(ball.pos);
        if (ball.trail.size() > ball.max_trail_length) {
          ball.trail.erase(ball.trail.begin());
        }
      }

      vec2 forces = {0, G};
      // float acc = G / ball.mass;
      vec2 acc = forces / ball.mass;
      // ball.vel.y += acc * dt;
      ball.vel += acc * dt;
      ball.pos += ball.vel * dt;

      if (IsKeyDown(KEY_SPACE)) {
        vec2 delta = mouse_pos - ball.pos;
        float distance = delta.length();
        vec2 direction = delta.norm();
        float force_magnitud = ATTRACT_FORCE * (distance / screen.x);
        vec2 force = direction * force_magnitud;

        ball.vel += force;
      }

      if (ball.pos.y + ball.radius >= screen.y) {
        ball.pos.y = screen.y - ball.radius;
        ball.vel.y *= -RESTITUTION;
      }

      if (ball.pos.x - ball.radius <= 0) {
        ball.pos.x = ball.radius;
        ball.vel.x *= -RESTITUTION;
      }
      if (ball.pos.x + ball.radius >= screen.x) {
        ball.pos.x = screen.x - ball.radius;
        ball.vel.x *= -RESTITUTION;
      }
    }

    for (size_t i = 0; i < objects.size(); i++) {
      for (size_t j = i + 1; j < objects.size(); j++) {
        resolve_collision(objects[i], objects[j]);
      }
    }

    BeginDrawing();
    ClearBackground(BACKGROUND);

    draw_grid(80);

    for (const auto& ball : objects) {
      if (debug) {
        for (size_t i = 1; i < ball.trail.size(); i++) {
          Color trail_color = Fade(ball.color, float(i) / ball.trail.size());
          DrawLineV(ball.trail[i - 1], ball.trail[i], trail_color);
        }
      }

      DrawCircleV(ball.pos, ball.radius, ball.color);
      DrawCircleLinesV(ball.pos, ball.radius, WHITE);

      if (debug) {
        vec2 scaled_vel = ball.vel * VELOCITY_VECTOR_SCALE;
        vec2 end_pos = ball.pos + scaled_vel;
        DrawLineV(ball.pos, end_pos, YELLOW);
        DrawCircleV(end_pos, 4, YELLOW);

        float speed = ball.vel.length();
        char label[64];
        snprintf(label, sizeof(label), "m: %.1f\nv: %.1f", ball.mass, speed);
        vec2 text_pos = {ball.pos.x + ball.radius + 5, ball.pos.y - 10};
        DrawText(label, text_pos.x, text_pos.y, 20, WHITE);
      }
    }

    DrawCircleV(mouse_pos, 10, RED);

    DrawFPS(0, 0);
    EndDrawing();
  }

  CloseWindow();

  return 0;
};
