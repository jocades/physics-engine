#pragma once

#include <raylib.h>

#include "../src/vec2.h"

class Demo {
 protected:
  virtual void update(float dt) = 0;
  virtual void draw() = 0;

 public:
  vec2 screen = {1280, 720};
  Color bg_color = GetColor(0x181818ff);

  Demo() {
    InitWindow(screen.x, screen.y, "Test");
    // SetTargetFPS(60);
    SetTargetFPS(144);
  }

  ~Demo() {
    CloseWindow();
  }

  void run() {
    while (!WindowShouldClose()) {
      update(GetFrameTime());
      BeginDrawing();
      ClearBackground(bg_color);
      draw();
      DrawFPS(0, 0);
      EndDrawing();
    }
  }
};
