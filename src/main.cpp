#include <iostream>

#include "rstartree.h"

using namespace std;

int main() {
    BoundingBox<RStarTraits<int, 4>> bb{{0, 1}, {3, 4}, {-1, 2}, {4, 5}};
    return 0;
}