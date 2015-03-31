#ifndef RSTARTREE_RSTARTRAITS_H
#define RSTARTREE_RSTARTRAITS_H

#include <stddef.h>
#include <bits/stl_pair.h>
#include "boundingbox.h"

template<typename T, size_t D, typename R, size_t m, size_t M>
struct RStarTraits {
  typedef T CoordType;
  static const size_t dimensions_ = D, min_size_ = m, max_size_ = M;
  typedef std::pair<T, T> Interval;
  typedef R RecordType;
  typedef BoundingBox<RStarTraits<T, D, R, m, M>> BBType;
};

#endif //BSTAR_TREE_RSTARTRAITS_H
