#include "way.h"

#include <cstdlib>
#include <vector>

Way::Way() : valid(0), dirty(0), tag(0), addr(0) {}

Way::Way(int _valid, int _dirty, int _tag, int _addr) :
    valid(_valid),
    dirty(_dirty),
    tag(_tag),
    addr(_addr) {}

Way::~Way() {}

void Way::setValid(int _valid) {
  valid = _valid;
}

void Way::setDirty(int _dirty) {
  dirty = _dirty;
}

void Way::setTag(int _tag) {
  tag = _tag;
}

void Way::setAddr(int _addr) {
  addr = _addr;
}

void Way::setTime(int _time) {
  time = _time;
}

int Way::readValid() {
  return valid;
}

int Way::readDirty() {
  return dirty;
}

int Way::readTag() {
  return tag;
}

int Way::readAddr() {
  return addr;
}

int Way::readTime() {
  return time;
}

int Way::isDirty() {
  return dirty;
}
