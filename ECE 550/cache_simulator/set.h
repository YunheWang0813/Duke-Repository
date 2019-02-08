#include <cstdlib>
#include <vector>

#include "way.h"
class Set
{
 private:
  int A;  // associativity
  int B;  // block size
  std::vector<Way> ways;

 public:
  Set(int a, int b);

  // check if the given tag can be hit in this set. If it hits, return the index of the way that hits. If it misses, return -1
  int hit(int tag);

  // return the index of the lru way
  int lru();

  Way & operator[](int i);
};
