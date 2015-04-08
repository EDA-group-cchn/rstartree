#include <iostream>
#include <cassert>

#include "rstartree.h"

using namespace std;

void testing() {
  RStarTree<> rtree;
  RStarTree<>::BoundingBox bb1{{1, 3}, {2, 5}}, bb2{{1, 4}, {1, 2}},
      bb3{{2, 3}, {3, 4}};

  assert(bb1.HyperArea() == 6);
  assert(bb2.HyperArea() == 3);
  assert(bb3.HyperArea() == 1);

  assert(bb1 % bb2);
  assert(bb1 % bb3);
  assert(not (bb2 % bb3));

  assert(bb1.Intersect(bb2) == 0);
  assert(bb1.Intersect(bb3) == 1);
  assert(bb2.Intersect(bb3) == 0);

  assert(rtree.Intersect(bb1).size() == 0);
}

int main() {
  testing();
  return 0;
}
