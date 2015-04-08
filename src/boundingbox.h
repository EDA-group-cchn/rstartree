#ifndef RSTARTREE_BOUNDINGBOX_H_
#define RSTARTREE_BOUNDINGBOX_H_

#include <initializer_list>
#include <utility>

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
    area_ = -1;
  }
  void SetIntervalStart(size_t dimension, CoordType coord) {
    intervals_[dimension].first = coord;
    area_ = -1;
  }
  void SetIntervalEnd(size_t dimension, CoordType coord) {
    intervals_[dimension].second = coord;
    area_ = -1;
  }
  CoordType HyperArea();

 private:
  Interval intervals_[dimensions_];
  CoordType area_;
};

template <typename T>
BoundingBox<T>::BoundingBox(std::initializer_list<typename BoundingBox::Interval> intervals) {
  size_t i = 0;
  if (intervals.size() != dimensions_)
    throw RStarException("Dimensions mismatch");
  for (const Interval &interval : intervals) {
    intervals_[i] = interval;
    ++i;
  }
  area_ = -1;
}

template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>::HyperArea() {
  BoundingBox<T>::CoordType res = 1;
  if (area_ != -1)
    return this->area_ = res;
  for (const typename BoundingBox<T>::Interval &interval : BoundingBox<T>::intervals_) {
    res *= interval.second - interval.first;
  }
  return this->area_ = res;
}


#endif //RSTARTREE_BOUNDINGBOX_H_


