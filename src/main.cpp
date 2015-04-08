#include <iostream>

#include "rstartree.h"

using namespace std;

int main() {
  RStarTree<> rtree;
  RStarTree<>::BoundingBox bb{{1, 20}, {2, 5}};
  RStarTree<>::BoundingBox bb2{{2, 8}, {3, 4}};
  bb.HyperArea();
  bb.Intersect(bb2);
  return 0;
}
