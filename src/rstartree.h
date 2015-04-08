#ifndef RSTARTREE_RSTARTREE_H_
#define RSTARTREE_RSTARTREE_H_

#include <vector>
#include "rstartraits.h"

template<typename Traits=RStarTraits<double, 2, size_t>>
class RStarTree {
 public:
  typedef typename Traits::BBType BoundingBox;
 private:
  typedef typename Traits::CoordType CoordType;
  static const size_t dimensions_ = Traits::dimensions_;
  typedef typename Traits::Interval Interval;
  typedef typename Traits::RecordType RecordType;

  struct Node {
    typedef std::pair<BoundingBox, Node *> ChildEntry;
    typedef std::pair<BoundingBox, RecordType> RecordEntry;
    std::vector<ChildEntry> children_;
    std::vector<RecordEntry> records_;
    size_t level_;
  };

 public:
  RStarTree(size_t min_node_size=20, size_t max_node_size=50) :
      root_(new Node), min_node_size_(min_node_size_),
      max_node_size_(max_node_size) {
  }
 private:
  Node *root_;
  size_t min_node_size_, max_node_size_;
};

#endif //RSTARTREE_RSTARTREE_H_
