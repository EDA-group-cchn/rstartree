#ifndef RSTARTREE_RSTARTREE_H_
#define RSTARTREE_RSTARTREE_H_

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <functional>

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
  typedef std::vector<Entry> VEntry;

  struct Node {
    VEntry children_;
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
  BoundingBox BuildBoundingBox(typename VEntry::iterator begin,
                               typename VEntry::iterator end);
  typename VEntry::iterator FindEntry(VEntry &entries, Node *node);
  void InsertEntry(const Entry &entry, size_t level);
  Node *&ChooseSubtree(const BoundingBox &bounding_box, size_t level);
  bool OverflowTreatment(Node *node);
  std::pair<Node *, Node *> Split(Node *node);
  void ReInsert(Node *node);

  /* Delete subroutines */
  Node *FindLeaf(const BoundingBox &bounding_box, const RecordType &record,
                 Node *node);
  void CondenseTree(Node *node);

  Node *root_;
  size_t min_node_size_, max_node_size_;
  std::set<size_t> overflown_levels_;
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

/* Insertion */
template <typename T>
void RStarTree<T>::Insert(const BoundingBox &bounding_box,
                          RecordType record) {
  Entry entry{bounding_box, static_cast<void *>(new RecordType(record))};
  InsertEntry(entry, 0);
}

template <typename T>
void RStarTree<T>::InsertEntry(const Entry &entry, size_t level) {
}

template <typename T>
typename RStarTree<T>::BoundingBox RStarTree<T>::BuildBoundingBox(
    typename VEntry::iterator begin, typename VEntry::iterator end) {
  BoundingBox res;
  for (auto it = begin; it != end; ++it)
    res += it->first;
  return res;
}

template <typename T>
typename RStarTree<T>::VEntry::iterator
RStarTree<T>::FindEntry(VEntry &entries, Node *node) {
  typename VEntry::iterator it;
  for (it = entries.begin(); it != entries.end(); ++it) {
    if (static_cast<Node *>(it->second) == node)
      break;
  }
  return it;
}

template <typename T>
bool RStarTree<T>::OverflowTreatment(Node *node) {
  if (node != root_ and
      overflown_levels_.find(node->level_) != overflown_levels_.end()) {
    overflown_levels_.insert(node->level_);
    ReInsert(node);
    return false;
  } else {
    Node *&parent = node->parent_;
    if (!parent) {
      // We have to add a new level
      parent = new Node(node->level_ + 1);
      root_ = parent;
    } else {
      parent->children_.erase(FindEntry(parent->children_, node));
    }
    auto split = Split(node);
    parent->children_.push_back(
        {BuildBoundingBox(split.first->children_.begin(),
                          split.first->children_.end()),
         split.first});
    parent->children_.push_back(
        {BuildBoundingBox(split.second->children_.begin(),
                          split.second->children_.end()),
         split.second});
    return true;
  }
}

template <typename T>
std::pair<typename RStarTree<T>::Node *, typename RStarTree<T>::Node *>
    RStarTree<T>::Split(Node *node) {
  using namespace std::placeholders;
  auto order = [] (const Entry &a, const Entry &b, size_t d,
                   CoordType (BoundingBox::*getter)(size_t)) {
      return (a.first.*getter)(d) < (b.first.*getter)(d);
  };
  /* Choosing axis */
  size_t best_axis = 0;
  CoordType best_s = std::numeric_limits<CoordType>::max(), best_a = best_s;
  VEntry &children = node->children_;
  for (size_t i = 0; i < dimensions_; ++i) {
    auto search_distribution = [&children, &best_s, &best_axis, this]
            (size_t axis) {
        CoordType s;
        for (size_t m = this->min_node_size_;
             m + this->min_node_size_ <= children.size(); ++m) {
          auto mid_it = children.begin() + m;
          s = BuildBoundingBox(children.begin(), mid_it).Margin() +
              BuildBoundingBox(mid_it, children.end()).Margin();
          if (s < best_s) {
            best_axis = axis;
            best_s = s;
          }
        }
    };
    std::sort(children.begin(), children.end(),
              std::bind(order, _1, _2, i, &BoundingBox::GetIntervalStart));
    search_distribution(i);
    std::sort(children.begin(), children.end(),
              std::bind(order, _1, _2, i, &BoundingBox::GetIntervalEnd));
    search_distribution(i);
  }
  best_s = std::numeric_limits<CoordType>::max();
  bool best_orientation;
  size_t best_partition;
  auto search_distribution =
          [&children, &best_s, &best_a, &best_orientation, &best_partition, this]
          (bool orientation) {
      CoordType s, a;
      for (size_t m = min_node_size_; m + min_node_size_ <= children.size();
           ++m) {
        auto mid_it = children.begin() + m;
        BoundingBox bb1 = BuildBoundingBox(children.begin(), mid_it),
            bb2 = BuildBoundingBox(mid_it, children.end());
        s = bb1.Overlap(bb2);
        a = bb1.Area() + bb2.Area();
        if (s < best_s or (s == best_s and a < best_a)) {
          best_s = s;
          best_a = a;
          best_orientation = orientation;
          best_partition = m;
        }
      }
  };
  std::sort(children.begin(), children.end(),
            std::bind(order, _1, _2, best_axis, &BoundingBox::GetIntervalStart));
  search_distribution(0);
  std::sort(children.begin(), children.end(),
            std::bind(order, _1, _2, best_axis, &BoundingBox::GetIntervalEnd));
  search_distribution(1);
  sort(children.begin(), children.end(), best_orientation ?
      std::bind(order, _1, _2, best_axis, &BoundingBox::GetIntervalEnd) :
      std::bind(order, _1, _2, best_axis, &BoundingBox::GetIntervalStart));
  std::pair<Node *, Node *> res = {
      new Node(node->level_, node->parent_),
      new Node(node->level_, node->parent_)
  };
  auto mid_it = children.begin() + best_partition;
  res.first->children_.assign(children.begin(), mid_it);
  res.second->children_.assign(mid_it, children.end());
  delete node;
  return res;
}

template <typename T>
void RStarTree<T>::ReInsert(Node *node) {
  VEntry &children = node->children_;
  BoundingBox full_bb = BuildBoundingBox(children.begin(), children.end());
  std::sort(children.begin(), children.end(),
            [&full_bb] (const Entry &a, const Entry &b) {
      return full_bb.CenterDistance(a.first) < full_bb.CenterDistance(b.first);
  });
  size_t p = static_cast<size_t>(0.3 * max_node_size_);
  VEntry tmp(children.end()-p, children.end());
  children.erase(children.end()-p, children.end());
  FindEntry(node->parent_->children_, node)->first =
      BuildBoundingBox(children.begin(), children.end());
  for (Entry &entry : tmp) {
    Insert(entry, node->level_);
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
