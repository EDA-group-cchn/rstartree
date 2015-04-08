#ifndef RSTARTREE_RSTARTRAITS_H_
#define RSTARTREE_RSTARTRAITS_H_

#include <utility>

#include "boundingbox.h"

template<typename T=double, size_t D=2, typename R=size_t>
struct RStarTraits {
  typedef T CoordType;
  static const size_t dimensions_ = D;
  typedef std::pair<T, T> Interval;
  typedef R RecordType;
  typedef BoundingBox<RStarTraits<T, D, R>> BBType;
};

#endif //RSTARTREE_RSTARTRAITS_H_

