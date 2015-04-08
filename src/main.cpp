#include <iostream>

#include "rstartree.h"

using namespace std;

int main() {
  RStarTree<> rtree;
  RStarTree<>::BoundingBox bb{{1, 3}, {2, 4}};
  bb.HyperArea();
  return 0;
}
