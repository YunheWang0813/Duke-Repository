#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

#include "cache.h"

class Simulator
{
 private:
  /* if mode of l1 is UNIFIED, use l1_I$ to represent the 
   * l1 cache. if mode of l2 is UNIFIED, use l2_I$ to represent
   * the l2 cache.
   */
  Cache l1_I$;
  Cache l1_D$;
  Cache l2_I$;
  Cache l2_D$;
  vector<int> codes;
  vector<int> addrs;

 public:
  Simulator(int * l1_I$_p, int * l1_D$_p, int * l2_I$_p, int * l2_D$_p);

  void FillInput();

  vector<int> Convert_and_Parse(const string & temp);

  void run(int l1_mode, int l2_mode);

  void print(int l2_mode);
};
