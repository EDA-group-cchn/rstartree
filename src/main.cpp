#include <iostream>
#include <cassert>

#include "rstartree.h"

using namespace std;

void testing() {
  RStarTree<> rtree;
  RStarTree<>::BoundingBox bb1{{1, 3}, {2, 5}}, bb2{{1, 4}, {1, 2}},
      bb3{{2, 3}, {3, 4}};
  
  RStarTree<>::BoundingBox s1,s2;
  RStarTree<>::BoundingBox r1{{1,4},{1,5}}, r2{{1,4},{1,4}}, r3{{1,4},{1,5}};
  s1 = bb1+bb2;
  s2 = bb2+bb3;
  
  // test sum for s1
  assert(s1.GetIntervalStart(0) == r1.GetIntervalStart(0));
  assert(s1.GetIntervalStart(1) == r1.GetIntervalStart(1));
  assert(s1.GetIntervalEnd(0) == r1.GetIntervalEnd(0));
  assert(s1.GetIntervalEnd(1) == r1.GetIntervalEnd(1));
  // test sum for s2
  assert(s2.GetIntervalStart(0) == r2.GetIntervalStart(0));
  assert(s2.GetIntervalStart(1) == r2.GetIntervalStart(1));
  assert(s2.GetIntervalEnd(0) == r2.GetIntervalEnd(0));
  assert(s2.GetIntervalEnd(1) == r2.GetIntervalEnd(1));
    
  r1+=r2;
  // test += 
  assert(r1.GetIntervalStart(0) == r3.GetIntervalStart(0));
  assert(r1.GetIntervalStart(1) == r3.GetIntervalStart(1));
  assert(r1.GetIntervalEnd(0) == r3.GetIntervalEnd(0));
  assert(r1.GetIntervalEnd(1) == r3.GetIntervalEnd(1));

  assert(bb1.HyperArea() == 6);
  assert(bb2.HyperArea() == 3);
  assert(bb3.HyperArea() == 1);

  assert(bb1 % bb2);
  assert(bb1 % bb3);
  assert(not (bb2 % bb3));

  assert(bb1.Overlap(bb2) == 0);
  assert(bb1.Overlap(bb3) == 1);
  assert(bb2.Overlap(bb3) == 0);

  assert(rtree.Intersect(bb1).size() == 0);

}

int main() {
  testing();
  return 0;
}
