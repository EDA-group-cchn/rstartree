#ifndef RSTARTREE_RSTARTREE_H
#define RSTARTREE_RSTARTREE_H

#include <vector>
#include "rstartraits.h"

template<typename Traits=RStarTraits<double, 2, size_t, 20, 50>>
class RStarTree {
  typedef typename Traits::CoordType CoordType;
  static const size_t dimensions_ = Traits::dimensions_;
  typedef typename Traits::Interval Interval;
  typedef typename Traits::RecordType RecordType;
 public:
  typedef typename Traits::BBType BoundingBox;

 private:
  struct Node {
    typedef std::pair<BoundingBox, Node *> ChildEntry;
    typedef std::pair<BoundingBox, RecordType> RecordEntry;
    std::vector<Node *> children_;
    std::vector<RecordType> record_ids_;
  };
};

#endif //RSTARTREE_RSTARTREE_H
