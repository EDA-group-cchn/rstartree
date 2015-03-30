#ifndef RSTARTREE_RSTARTRAITS_H
#define RSTARTREE_RSTARTRAITS_H

#include <stddef.h>
#include <bits/stl_pair.h>

template<typename T, size_t D>
struct RStarTraits {
    typedef T CoordType;
    static const size_t dimensions = D;
    typedef std::pair<T, T> Interval;
};

#endif //BSTAR_TREE_RSTARTRAITS_H
