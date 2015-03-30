//
// Created by alculquicondor on 3/29/15.
//

#ifndef RSTARTREE_BOUNDINGBOX_H
#define RSTARTREE_BOUNDINGBOX_H

#include <stddef.h>
#include "rstarexception.h"

template <typename Traits>
class BoundingBox {
    typedef typename Traits::CoordType CoordType;
    static const size_t dimensions = Traits::dimensions;
    typedef typename Traits::Interval Interval;
public:
    BoundingBox(std::initializer_list<Interval> intervals);

    Interval GetInterval(size_t dimension) const {
        return _intervals[dimension];
    }
    CoordType GetIntervalStart(size_t dimension) const {
        return _intervals[dimension].first;
    }
    CoordType GetIntervalEnd(size_t dimension) const {
        return _intervals[dimension].second;
    }

    void SetInterval(size_t dimension, Interval interval) {
       _intervals[dimension] = interval;
    }
    void SetIntervalStart(size_t dimension, CoordType coord) {
        _intervals[dimension].first = coord;
    }
    void SetIntervalEnd(size_t dimension, CoordType coord) {
        _intervals[dimension].second = coord;
    }

private:
    Interval _intervals[dimensions];
};

template <typename T>
BoundingBox<T>::BoundingBox(std::initializer_list<typename BoundingBox::Interval> intervals) {
    size_t i = 0;
    if (intervals.size() != dimensions)
        throw RStarException("Dimensions mismatch");
    for (const Interval &interval : intervals)
        _intervals[i] = interval;
}

#endif //BSTAR_TREE_BOUNDINGBOX_H

