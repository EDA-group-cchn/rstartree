#ifndef RSTARTREE_BOUNDINGBOX_H_
#define RSTARTREE_BOUNDINGBOX_H_

#include <initializer_list>

#include "rstarexception.h"

template <typename Traits>
class BoundingBox {
  typedef typename Traits::CoordType CoordType;
  static const size_t dimensions_ = Traits::dimensions_;
  typedef typename Traits::Interval Interval;
 public:
  BoundingBox();
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
  CoordType Enlargement(const BoundingBox<Traits> &bb);
  CoordType Overlap(const BoundingBox<Traits> &bb) const;
  CoordType Margin();
  
  BoundingBox Extend(BoundingBox<Traits> &bb) const;
  BoundingBox operator + (BoundingBox<Traits> &bb) const;
  BoundingBox & operator += (const BoundingBox<Traits> &bb);

  CoordType CenterDistance (const BoundingBox<Traits>& bb);



  bool Intersects(const BoundingBox<Traits> &bounding_box) const;
  bool operator%(const BoundingBox<Traits> &bounding_box) const {
    return Intersects(bounding_box);
  }

  bool Equal(const BoundingBox<Traits> &bounding_box) const;
  bool operator==(const BoundingBox<Traits> &bounding_box) const {
    return Equal(bounding_box);
  }

 private:
  Interval intervals_[dimensions_];
  CoordType area_;
};

template <typename T>
BoundingBox<T>::BoundingBox() {
  area_ = -1;
}
template <typename T>
BoundingBox<T>::BoundingBox(
    std::initializer_list<Interval> intervals) {
  size_t i = 0;
  if (intervals.size() > dimensions_)
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
  for (const typename BoundingBox<T>::Interval &interval :
      BoundingBox<T>::intervals_) {
    res *= interval.second - interval.first;
  }
  return this->area_ = res;
}

template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>::Enlargement(
    const BoundingBox<T> &bb) {
  BoundingBox<T>::CoordType res = 1;
  for (size_t i = 0; i < T::dimensions_; ++i) {
    res *= std::max(this->intervals_[i].second, bb.intervals_[i].second) -
           std::min(this->intervals_[i].first, bb.intervals_[i].first);
  }
  return res;
}

template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>::Overlap(
    const BoundingBox<T> &bb) const {
  BoundingBox<T>::CoordType res = 1;
  for (size_t i = 0; i < T::dimensions_; ++i) {
    res *= std::max(static_cast<CoordType>(0),
        std::min(this->intervals_[i].second,bb.intervals_[i].second) -
        std::max(this->intervals_[i].first , bb.intervals_[i].first));
  }
  return res;
}

template <typename T>
bool BoundingBox<T>::Intersects(BoundingBox<T> const &bounding_box) const {
  for (size_t i = 0; i < dimensions_; ++i) {
    if (this->intervals_[i].first > bounding_box.intervals_[i].second or
        this->intervals_[i].second < bounding_box.intervals_[i].first)
      return false;
  }
  return true;
}

template <typename T>
bool BoundingBox<T>::Equal(BoundingBox<T> const &bounding_box) const {
  for (size_t i = 0; i < dimensions_; ++i) {
    if (this->intervals_[i] != bounding_box.intervals_[i])
      return false;
  }
  return true;
}

template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>:: Margin(){
  BoundingBox<T>::CoordType res = 0;
  for (std::size_t i = 0; i < dimensions_; ++i)
    res += (intervals_[i].second - intervals_[i].first);
  return res;
}

template<typename T>
BoundingBox<T> BoundingBox<T>::Extend(BoundingBox<T> &bb) const
{
  BoundingBox tmp;
  for (size_t i = 0; i < dimensions_; ++i) {
    tmp.intervals_[i].first = std::min(this->intervals_[i].first, bb.intervals_[i].first);
    tmp.intervals_[i].second = std::max(this->intervals_[i].second, bb.intervals_[i].second);
  }
  tmp.area_ = -1;
  return tmp;
}
template<typename T>
BoundingBox<T> BoundingBox<T>::operator + (BoundingBox<T> &bb) const
{
  return Extend(bb);
}
template<typename T>
BoundingBox<T>& BoundingBox<T>::operator += (const BoundingBox<T> &bb)
{
 
  for (size_t i = 0; i < dimensions_; ++i) {
    this->intervals_[i].first = std::min(this->intervals_[i].first, bb.intervals_[i].first);
    this->intervals_[i].second = std::max(this->intervals_[i].second, bb.intervals_[i].second);
  }
  this->area_ = -1;
  return *this;
}


template <typename T>
typename BoundingBox<T>::CoordType BoundingBox<T>:: CenterDistance (const BoundingBox<T>& bb){
BoundingBox<T>::CoordType distance = 0 , t ;
  for (std::size_t i = 0; i < dimensions_; ++i) {
    t = (this->intervals_[i].first + this->intervals_[i].second) / 2.0 -
        (bb.intervals_[i].first + bb.intervals_[i].second) /2.0;
    distance += t*t ;
  }
  return distance;
}


#endif //RSTARTREE_BOUNDINGBOX_H_
