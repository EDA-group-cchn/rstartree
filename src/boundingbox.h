#ifndef RSTARTREE_BOUNDINGBOX_H_
#define RSTARTREE_BOUNDINGBOX_H_

#include <stddef.h>
#include <initializer_list>
#include <utility>      // std::pair, std::get


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
  CoordType HyperArea() const;

 private:
  Interval intervals_[dimensions_];
};

template <typename T>
BoundingBox<T>::BoundingBox(std::initializer_list<typename BoundingBox::Interval> intervals) {
  size_t i = 0;
  if (intervals.size() != dimensions_)
    throw RStarException("Dimensions mismatch");
  for (const Interval &interval : intervals)
    {
        intervals_[i] = interval;
        ++i;
    }
}

template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>::HyperArea() const
{
    //if (BoundingBox<T>::Intervals.size() == 0)
    //    throw RStarException("No dimensions found");
    size_t i = 0;
    BoundingBox<T>::CoordType res = 1;
    for (const typename BoundingBox<T>::Interval &interval : BoundingBox<T>::intervals_)
    {
        res *= ( std::get<1>(GetInterval(i)) ) - ( std::get<0>(GetInterval(i)) );
        //std::cout << std::get<0>(GetInterval(i)) << std::endl;
        ++i;
    }
    //std::cout << res << std::endl;
    return res;

}


#endif //RSTARTREE_BOUNDINGBOX_H_


