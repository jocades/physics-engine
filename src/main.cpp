#include <raylib.h>

#include <cmath>
#include <string>
#include <vector>

#include "vec2.h"

const vec2 screen = {1280, 720};
const Color BACKGROUND = GetColor(0x282828ff);

const float G = 500.0f;          // px/s^2
const float RESTITUTION = 0.7f;  // Bounciness (0 = no bounce, 1 = perfect bounce)
const float VELOCITY_VECTOR_SCALE = 0.2f;
const float ATTRACT_FORCE = 250.0f;

bool debug = true;

struct Object {
  vec2 pos;
  vec2 vel;
  float mass;

  Object(vec2 pos, vec2 vel, float mass) : pos(pos), vel(vel), mass(mass) {}
};

struct Circle : public Object {
  float radius;
  Color color;
  std::vector<vec2> trail;
  const size_t max_trail_length = 30;
  std::string label;

  Circle(vec2 pos, vec2 vel, float mass, float radius, Color color)
      : Object(pos, vel, mass), radius(radius), color(color) {}
};

void resolve_collision(Circle& a, Circle& b) {
  vec2 delta = b.pos - a.pos;
  // float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);
  float distance = delta.length();
  float min_dist = a.radius + b.radius;

  if (distance < min_dist && distance > 0.0f) {
    vec2 norm = delta.norm();
    // vec2 relative_vel = b.vel - a.vel;
    // float vel_along_normal = relative_vel.x * norm.x + relative_vel.y * norm.y;
    // float vel_along_normal = relative_vel.dot(norm);
    float vel_along_normal = (b.vel - a.vel).dot(norm);
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

int main() {
  InitWindow(screen.x, screen.y, "Physics Engine");
  SetTargetFPS(60);

  std::vector<Circle> objects = {
    Circle({300, 100}, {100, 50}, 2.0f, 30, RED),
    Circle({500, 150}, {-80, 0}, 4.0f, 40, BLUE),
    Circle({700, 200}, {-50, -30}, 3.0f, 35, GREEN),
  };

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    for (auto& ball : objects) {
      ball.trail.push_back(ball.pos);
      if (ball.trail.size() > ball.max_trail_length) {
        ball.trail.erase(ball.trail.begin());
      }

      ball.vel.y += G * dt;
      ball.pos += ball.vel * dt;

      if (IsKeyDown(KEY_SPACE)) {
        vec2 mouse_pos = GetMousePosition();
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

    int cell_size = 80;
    for (int x = 0; x < screen.x / cell_size; x++) {
      DrawLine(x * cell_size, 0, x * cell_size, screen.y, Fade(LIGHTGRAY, 0.25));
    }
    for (int y = 0; y < screen.y / cell_size; y++) {
      DrawLine(0, y * cell_size, screen.x, y * cell_size, Fade(LIGHTGRAY, 0.25));
    }

    for (const auto& ball : objects) {
      for (size_t i = 1; i < ball.trail.size(); i++) {
        Color trail_color = Fade(ball.color, float(i) / ball.trail.size());
        DrawLineV(ball.trail[i - 1], ball.trail[i], trail_color);
      }

      DrawCircleV(ball.pos, ball.radius, ball.color);
      DrawCircleLinesV(ball.pos, ball.radius, WHITE);

      if (debug) {
        vec2 scaled_vel = ball.vel * VELOCITY_VECTOR_SCALE;
        vec2 end_pos = ball.pos + scaled_vel;
        DrawLineV(ball.pos, end_pos, YELLOW);
        DrawCircleV(end_pos, 4, YELLOW);

        float speed = sqrt(ball.vel.x * ball.vel.x + ball.vel.y * ball.vel.y);
        char label[64];
        snprintf(label, sizeof(label), "m: %.1f\nv: %.1f", ball.mass, speed);
        vec2 text_pos = {ball.pos.x + ball.radius + 5, ball.pos.y - 10};
        DrawText(label, text_pos.x, text_pos.y, 10, WHITE);
      }
    }

    DrawFPS(0, 0);
    EndDrawing();
  }

  CloseWindow();

  return 0;
};
