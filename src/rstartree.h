#ifndef RSTARTREE_RSTARTREE_H_
#define RSTARTREE_RSTARTREE_H_

#include <vector>
#include <set>

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
  typedef std::pair<BoundingBox, void *> Entry;  // holds nodes and/or records

  struct Node {
    std::vector<Entry> children_;
    size_t level_;
    Node *parent_;
    Node(size_t level=0, Node *parent=nullptr) :
        level_(level), parent_(parent) {
    }
  };

 public:
  RStarTree(size_t min_node_size=20, size_t max_node_size=50) :
      root_(new Node), min_node_size_(min_node_size_),
      max_node_size_(max_node_size) {
  }

  std::vector<RecordType> Intersect(const BoundingBox &bounding_box);

  void Insert(const BoundingBox &bounding_box, RecordType record);

  void Delete(const BoundingBox &bounding_box, RecordType record);

 private:
  /* Insert subroutines */
  void Insert(const Entry &entry, size_t level);
  Node *&ChooseSubtree(const BoundingBox &bounding_box, size_t level);
  bool OverflowTreatment(Node *node);
  void Split(Node *node);
  void ReInsert(Node *node);

  /* Delete subroutines */
  Node *FindLeaf(const BoundingBox &bounding_box, RecordType record);
  void CondenseTree(Node *node);

  Node *root_;
  size_t min_node_size_, max_node_size_;
  std::set<size_t> overflown_levels;
};

#endif //RSTARTREE_RSTARTREE_H_
