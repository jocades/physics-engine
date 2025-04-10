#include <iostream>
#include <vector>

struct Base {
  int n;

  Base(int n) : n(n) {}

  virtual void fn() = 0;
};

struct A : Base {
  A(int n) : Base(n) {}

  void fn() override {
    std::cout << "A\n";
  }
};

struct B : Base {
  B(int n) : Base(n) {}

  void fn() override {
    std::cout << "B\n";
  }
};

void add(Base& b) {}

int main() {
  std::vector<Base*> xs;

  A a(1);
  B b(2);

  add(a);

  xs.push_back(&a);
  xs.push_back(&b);

  Base* base = xs[0];

  B* bp = (B*)base;
  bp->fn();  // treating 'A' as if it was 'B', not right.

  // B* bp = dynamic_cast<B*>(base);
  // bp->fn(); // segmentation fault;
}
