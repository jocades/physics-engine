#include <format>

#include "object.h"

class Engine {
 public:
  std::vector<Object> objects;

  void add_object() {
    // objects.emplace_back(obj);
  }

  // void remove_object(Object* obj) {
  //   if (!obj) return;
  //   auto it = std::find(objects.begin(), objects.end(), obj);
  //   if (it == objects.end()) return;
  //   objects.erase(it);
  // }
};
