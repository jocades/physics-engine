#include "../src/vec2.h"
#include <raylib.h>
#include <iostream>

using std::cout;

int main() {
  vec2 a(1, 1);
  vec2 b(3, 3);
  cout << a << '\n';
  printf("length=%f distance=%f\n", a.length(), a.distance(b));
}
