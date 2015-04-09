#ifndef RSTARTREE_RSTARTREE_H_
#define RSTARTREE_RSTARTREE_H_

#include <vector>
#include <set>

#include "rstartraits.h"

template <typename Traits=RStarTraits<double, 2, size_t>>
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
  /* Search subroutines */
  void Search(const BoundingBox &bounding_box, Node *node,
      bool (BoundingBox::*criteria)(const BoundingBox &) const);

  /* Insert subroutines */
  void Insert(const Entry &entry, size_t level);
  Node *&ChooseSubtree(const BoundingBox &bounding_box, size_t level);
  bool OverflowTreatment(Node *node);
  void Split(Node *node);
  void ReInsert(Node *node);

  /* Delete subroutines */
  Node *FindLeaf(const BoundingBox &bounding_box, const RecordType &record,
                 Node *node);
  void CondenseTree(Node *node);

  Node *root_;
  size_t min_node_size_, max_node_size_;
  std::set<size_t> overflown_levels;
  std::vector<RecordType> results_buffer_;
};

/* Search */
template <typename T>
std::vector<typename RStarTree<T>::RecordType> RStarTree<T>::Intersect(
    BoundingBox const &bounding_box) {
  results_buffer_.clear();
  Search(bounding_box, root_, &BoundingBox::Intersects);
  return results_buffer_;
}

template <typename T>
void RStarTree<T>::Search(BoundingBox const &bounding_box, Node *node,
    bool (BoundingBox::*criteria)(const BoundingBox &) const) {
  if (node->level_ == 0) {
    for (Entry &entry : node->children_) {
      if ((bounding_box.*criteria)(entry.first))
        results_buffer_.push_back(*static_cast<RecordType *>(entry.second));
    }
  } else {
    for (Entry &entry : node->children_) {
      if (bounding_box % entry.first)
        Search(bounding_box, static_cast<Node *>(entry.second), criteria);
    }
  }
}

/* Deletion */
template <typename T>
typename RStarTree<T>::Node *RStarTree<T>::FindLeaf(
    const BoundingBox &bounding_box, const RecordType &record, Node *node) {
  if (node->level_ == 0) {
    for (Entry &entry : node->children_) {
      if (bounding_box == entry.first and
          record == *static_cast<RecordType *>(entry.second))
        return node;
    }
    return nullptr;
  } else {
    Node *res;
    for (Entry &entry : node->children_)
      if (bounding_box % entry.first) {
        res = FindLeaf(bounding_box, record, static_cast<Node *>(entry.second));
        if (res != nullptr)
          return res;
      }
    return nullptr;
  }
}

#endif //RSTARTREE_RSTARTREE_H_

