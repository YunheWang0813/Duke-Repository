#include <cstdlib>
#include <vector>

class Way
{
 private:
  int valid;
  int dirty;
  int tag;
  int addr;
  int time;

 public:
  Way();
  Way(int _valid, int _dirty, int tag, int addr);
  ~Way();

  void setValid(int _valid);
  void setDirty(int _dirty);
  void setTag(int _tag);
  void setAddr(int _addr);
  void setTime(int _time);
  int readValid();
  int readDirty();
  int readTag();
  int readAddr();
  int readTime();

  int isDirty();
};
