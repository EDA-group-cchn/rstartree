#ifndef RSTARTREE_BOUNDINGBOX_H_
#define RSTARTREE_BOUNDINGBOX_H_

#include <stddef.h>
#include <initializer_list>

#include "rstarexception.h"

template <typename Traits>
class BoundingBox {
  typedef typename Traits::CoordType CoordType;
  static const size_t dimensions_ = Traits::dimensions_;
  typedef typename Traits::Interval Interval;
 public:
  BoundingBox(std::initializer_list<Interval> intervals);

  Interval GetInterval(size_t dimension) const {
    return intervals_[dimension];
  }
  CoordType GetIntervalStart(size_t dimension) const {
    return intervals_[dimension].first;
  }
  CoordType GetIntervalEnd(size_t dimension) const {
    return intervals_[dimension].second;
  }

  void SetInterval(size_t dimension, Interval interval) {
    intervals_[dimension] = interval;
  }
  void SetIntervalStart(size_t dimension, CoordType coord) {
    intervals_[dimension].first = coord;
  }
  void SetIntervalEnd(size_t dimension, CoordType coord) {
    intervals_[dimension].second = coord;
  }

 private:
  Interval intervals_[dimensions_];
};

template <typename T>
BoundingBox<T>::BoundingBox(std::initializer_list<typename BoundingBox::Interval> intervals) {
  size_t i = 0;
  if (intervals.size() != dimensions_)
    throw RStarException("Dimensions mismatch");
  for (const Interval &interval : intervals)
    intervals_[i] = interval;
}

#endif //RSTARTREE_BOUNDINGBOX_H_


