#include "set.h"

#include <cstdlib>
#include <vector>

#include "limits.h"

Set::Set(int a, int b) : A(a), B(b), ways(std::vector<Way>(A, Way())) {}

int Set::hit(int tag) {
  for (int i = 0; i < A; i++) {
    if (ways[i].readValid() == 1 && ways[i].readTag() == tag) {
      return i;
    }
  }
  return -1;
}

int Set::lru() {
  int latest = INT_MAX;
  int index = -1;
  for (int i = 0; i < A; i++) {
    if (ways[i].readTime() < latest) {
      latest = ways[i].readTime();
      index = i;
    }
  }
  return index;
}

Way & Set::operator[](int i) {
  return ways[i];
}
