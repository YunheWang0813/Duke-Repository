#define NONEXIST 0
#define UNIFIED 1
#define SPLIT 2

#include "cache.h"

#include <cstdlib>
#include <iostream>
#include <vector>
int Cache::getIndex(int addr) {
  return (addr / B) % n_sets;
}

int Cache::getTag(int addr) {
  return addr / (B * n_sets);
}

Cache::Cache() {}

Cache::Cache(int a, int b, int c) :
    A(a),
    B(b),
    C(c),
    n_sets((C / B) / A),
    sets(std::vector<Set>(n_sets, Set(a, b))),
    nextI$(NULL),
    nextD$(NULL),
    n_read_access(0),
    n_read_miss(0),
    n_write_access(0),
    n_write_miss(0),
    n_insn_access(0),
    n_insn_miss(0) {
  //for (int i = 0; i < n_sets; i++) {
  //sets[i].printSet();
  //}
}

void Cache::setNextCaches(Cache * I$, Cache * D$) {
  nextI$ = I$;
  nextD$ = D$;
}

void Cache::access(int addr, int code, int next_level_mode, int clock) {
  int index = getIndex(addr);
  int tag = getTag(addr);

  // update access number
  if (code == 0) {
    n_read_access++;
  }
  else if (code == 1) {
    n_write_access++;
  }
  else {
    n_insn_access++;
  }

  int hit = sets[index].hit(tag);
  // if hit
  if (hit != -1) {
    // if code is data write
    if (code == 1) {
      // mark dirty
      sets[index][hit].setDirty(1);
    }
    sets[index][hit].setTime(clock);
    return;
  }

  // if miss

  // update miss number
  if (code == 0) {
    n_read_miss++;
  }
  else if (code == 1) {
    n_write_miss++;
  }
  else {
    n_insn_miss++;
  }

  // using LRU to get the way to evict
  // to be modified
  //  Way evict = sets[index][0];
  int evict = sets[index].lru();
  // if this way is dirty, write the ORIGINAL address in the way back to the lower level
  if (sets[index][evict].isDirty()) {
    int old_addr = sets[index][evict].readAddr();
    // if next level mode is NONE (no next level)
    if (next_level_mode == NONEXIST) {
      // do nothing
    }
    // if next level mode is UNIFIED
    else if (next_level_mode == UNIFIED) {
      // only use the insn cache in the next level
      nextI$->access(old_addr, 1, NONEXIST, clock);
    }
    // if next level mode is SPLIT
    else {
      // if the code is fetch insns
      if (code == 2) {
        // use the insn cache in next level
        nextI$->access(old_addr, 1, NONEXIST, clock);
      }
      // if the code is read/write data
      else {
        // use the data cache in next level
        nextD$->access(old_addr, 1, NONEXIST, clock);
      }
    }
  }

  // fill this way with the new address
  sets[index][evict].setValid(1);
  //  if (code == 1) {
  //sets[index][evict].setDirty(1);
  //}
  //else {
  sets[index][evict].setDirty(0);
  // }
  sets[index][evict].setTag(tag);
  sets[index][evict].setAddr(addr);
  sets[index][evict].setTime(clock);

  // recursively read the address from the lower level
  // if next level mode is NONE (no next level)
  if (next_level_mode == NONEXIST) {
    // do nothing
  }
  // if next level mode is UNIFIED
  else if (next_level_mode == UNIFIED) {
    // only use the insn cache in the next level
    if (code == 2) {
      nextI$->access(addr, 2, NONEXIST, clock);
    }
    else {
      nextI$->access(addr, 0, NONEXIST, clock);
    }
  }
  // if next level mode is SPLIT
  else {
    // if the code is fetch insns
    if (code == 2) {
      // use the insn cache in next level
      nextI$->access(addr, 2, NONEXIST, clock);
    }
    // if the code is read/write data
    else {
      // use the data cache in next level
      nextD$->access(addr, 0, NONEXIST, clock);
    }
  }
}

int Cache::getReadAccess() {
  return n_read_access;
}

int Cache::getReadMiss() {
  return n_read_miss;
}

int Cache::getWriteAccess() {
  return n_write_access;
}

int Cache::getWriteMiss() {
  return n_write_miss;
}

int Cache::getInsnAccess() {
  return n_insn_access;
}

int Cache::getInsnMiss() {
  return n_insn_miss;
}
