#include <iostream>
#include <cassert>
#include <cstdlib>

#include "rstartree.h"

using namespace std;

typename RStarTree<>::BoundingBox GetRandomBB() {
  double a = rand() % 10000, b = rand() % 10000;
  return {{a, a + rand() % 5}, {b, b + rand() % 5}};
}

void Testing() {
  RStarTree<> rtree(4, 10);
  RStarTree<>::BoundingBox bb1{{1, 3}, {2, 5}}, bb2{{1, 4}, {1, 2}},
      bb3{{2, 3}, {3, 4}};
  
  RStarTree<>::BoundingBox s1,s2;
  RStarTree<>::BoundingBox r1{{1,4},{1,5}}, r2{{1,4},{1,4}}, r3{{1,4},{1,5}};
  s1 = bb1+bb2;
  s2 = bb2+bb3;
  cout<<bb1.GetInterval(0).first<<endl;
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

  rtree.Insert(bb1, 1);
  rtree.Insert(bb2, 2);
  rtree.Insert(bb3, 3);

  rtree.Insert({{5, 6}, {5, 6}}, 4);
  rtree.Insert({{8, 9}, {8, 9}}, 5);
  rtree.Insert({{12, 13}, {12, 13}}, 6);
  rtree.Insert({{15, 16}, {15, 16}}, 7);


  rtree.Delete(bb1,0);


  assert(rtree.Intersect(bb1).size() == 3);
  assert(rtree.Intersect({{0, 20}, {0, 20}}).size() == 7);
  for (size_t i = 0; i < 4000; ++i) {
    RStarTree<>::BoundingBox bb = GetRandomBB();
    rtree.Insert(bb, 8 + i);
    assert(rtree.Intersect(bb).size() != 0);
  }

}

int main() {
  Testing();
  return 0;
}
