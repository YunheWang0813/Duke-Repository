#include <cstdlib>
#include <vector>

#include "set.h"

class Cache
{
 private:
  int A;  // associativity
  int B;  // block size
  int C;  // capacity
  int n_sets;
  std::vector<Set> sets;
  Cache * nextI$;
  Cache * nextD$;
  int n_read_access;
  int n_read_miss;
  int n_write_access;
  int n_write_miss;
  int n_insn_access;
  int n_insn_miss;

  int getIndex(int addr);  // get the index of set
  int getTag(int addr);

 public:
  Cache();
  Cache(int a, int b, int c);
  void setNextCaches(Cache * I$, Cache * D$);

  /* this function takes an address, a code and the mode for the
   * the next level. it tries to access this address in this 
   * cache. It it hits, return 1. If it misses, write this 
   * address to this cache, and tries to recursively access 
   * this address in the next level
   */
  void access(int addr, int code, int next_level_mode, int clock);
  int getReadAccess();
  int getReadMiss();
  int getWriteAccess();
  int getWriteMiss();
  int getInsnAccess();
  int getInsnMiss();
};
