#ifndef RSTARTREE_RSTARTREE_H_
#define RSTARTREE_RSTARTREE_H_

#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <functional>
#include <queue>          

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
    Node *parent_, *replacement_;
    Node(size_t level=0, Node *parent=nullptr) :
        level_(level), parent_(parent), replacement_(nullptr) {
    }
    Node *GetReplacement() {
      Node *node = this;
      while (node->replacement_)
        node = node->replacement_;
      return node;
    }
  };

 public:
  RStarTree(size_t min_node_size=20, size_t max_node_size=50) :
      root_(new Node), min_node_size_(min_node_size),
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
  BoundingBox BuildBoundingBox(typename VEntry::const_iterator begin,
                               typename VEntry::const_iterator end);
  BoundingBox BuildBoundingBox(const VEntry &entries);
  typename VEntry::iterator FindEntry(VEntry &entries, Node *node);
  void InsertEntry(const Entry &entry, size_t level);
  CoordType Overlap(const BoundingBox &bb, const VEntry &entries);
  Node *ChooseSubtree(const BoundingBox &bb, size_t level);
  bool OverflowTreatment(Node *node);
  std::pair<Node *, Node *> Split(Node *node);
  void ReInsert(Node *node);

  /* Delete subroutines */
  Node *FindLeaf(const BoundingBox &bb, const RecordType &record, Node *node);
  void CondenseTree(Node *node);

  Node *root_;
  size_t min_node_size_, max_node_size_;
  std::set<size_t> overflown_levels_;
  std::set<Node *> nodes_to_delete_;
  std::vector<RecordType> results_buffer_;
};

/* Search */
template <typename T>
std::vector<typename RStarTree<T>::RecordType> RStarTree<T>::Intersect(
    BoundingBox const &bounding_box) {
  results_buffer_.clear();
  Search(bounding_box, root_, &BoundingBox::Intersects);
  for (Node *node : nodes_to_delete_)
    delete node;
  nodes_to_delete_.clear();
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
  overflown_levels_.clear();
  Entry entry{bounding_box, static_cast<void *>(new RecordType(record))};
  InsertEntry(entry, 0);
}

template <typename T>
void RStarTree<T>::InsertEntry(const Entry &entry, size_t level) {
  Node *node = ChooseSubtree(entry.first, level), *parent;
  node->children_.push_back(entry);
  if (level != 0)
    static_cast<Node *>(entry.second)->parent_ = node;
  if (node->children_.size() > max_node_size_) {
    bool split = OverflowTreatment(node);
    node = node->GetReplacement();
    while (split and node != root_) {
      parent = node->parent_;
      if (parent->children_.size() > max_node_size_) {
        split = OverflowTreatment(parent);
        parent = parent->GetReplacement();
      } else {
        split = false;
      }
      node = parent;
    }
  }
  while (node != root_) {
    parent = node->parent_;
    typename VEntry::iterator it = FindEntry(parent->children_, node);
    it->first = BuildBoundingBox(node->children_);
    node = parent;
  }
}

template <typename T>
typename RStarTree<T>::CoordType RStarTree<T>::Overlap(BoundingBox const &bb,
                             RStarTree::VEntry const &entries) {
  CoordType result = 0;
  for (const Entry &e : entries)
    result += e.first.Overlap(bb);
  return result;
}

template <typename T>
typename RStarTree<T>::Node *RStarTree<T>::ChooseSubtree(
    const BoundingBox &bb, size_t level) {
  Node *node = root_, *best_child;
  typename VEntry::iterator it;
  CoordType cmp_best, cmp_value;
  while (node->level_ != level) {
    it = node->children_.begin();
    best_child = static_cast<Node *>(it->second);
    if (node->level_ == 1) {  // least overlap enlargement
      cmp_best = Overlap(bb + it->first, node->children_) -
          Overlap(it->first, node->children_);
      for (++it; it != node->children_.end(); ++it) {
        cmp_value = Overlap(bb + it->first, node->children_) -
                    Overlap(it->first, node->children_);
        if (cmp_value < cmp_best) {
          best_child = static_cast<Node *>(it->second);
          cmp_best = cmp_value;
        }
      }
    } else {  // least area enlargement
      cmp_best = it->first.Enlargement(bb) - it->first.HyperArea();
      for (++it; it != node->children_.end(); ++it) {
        cmp_value = it->first.Enlargement(bb) - it->first.HyperArea();
        if (cmp_value < cmp_best) {
          best_child = static_cast<Node *>(it->second);
          cmp_best = cmp_value;
        }
      }
    }
    node = best_child;
  }
  return node;
}

template <typename T>
typename RStarTree<T>::BoundingBox RStarTree<T>::BuildBoundingBox(
    typename VEntry::const_iterator begin,
    typename VEntry::const_iterator end) {
  typename VEntry::const_iterator it = begin;
  BoundingBox res = it->first;
  for (++it; it != end; ++it)
    res += it->first;
  return res;
}

template <typename T>
typename RStarTree<T>::BoundingBox RStarTree<T>::BuildBoundingBox(
    const VEntry &entries) {
  return BuildBoundingBox(entries.begin(), entries.end());
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
      overflown_levels_.find(node->level_) == overflown_levels_.end()) {
    overflown_levels_.insert(node->level_);
    ReInsert(node);
    return false;
  } else {
    Node *&parent = node->parent_;
    if (parent == nullptr) {
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
                   CoordType (BoundingBox::*getter)(size_t) const) {
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
        a = bb1.HyperArea() + bb2.HyperArea();
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
  if (node->level_ != 0) {
    for (Entry &e : res.first->children_)
      static_cast<Node *>(e.second)->parent_ = res.first;
    for (Entry &e : res.second->children_)
      static_cast<Node *>(e.second)->parent_ = res.second;
  }
  node->replacement_ = res.first;
  nodes_to_delete_.insert(node);
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
  size_t level = node->level_;
  Node *parent;
  while (node != root_) {
    parent = node->parent_;
    FindEntry(parent->children_, node)->first = BuildBoundingBox(children);
    node = parent;
  }
  for (Entry &entry : tmp)
    InsertEntry(entry, level);
}

/* Deletion */
template <typename T>
typename RStarTree<T>::Node *RStarTree<T>::FindLeaf(
    const BoundingBox &bb, const RecordType &record, Node *node) {
  if (node->level_ == 0) {
    for (Entry &entry : node->children_) {
      if (bb == entry.first and
          record == *static_cast<RecordType *>(entry.second))
        return node;
    }
    return nullptr;
  } else {
    Node *res;
    for (Entry &entry : node->children_)
      if (bb % entry.first) {
        res = FindLeaf(bb, record, static_cast<Node *>(entry.second));
        if (res != nullptr)
          return res;
      }
    return nullptr;
  }
}
template<typename T>
void RStarTree<T>::CondenseTree(Node *node)
{
  Node *tmp,*parent;
  tmp = node;
  std::queue<Node*> q;
 
  while(tmp != root_)
  {
    parent = tmp->parent_;
    if(tmp->children_.size()<min_node_size_)
    {
      typename VEntry::iterator it = FindEntry(parent->children_,tmp);
      parent->children_.erase(it);
      q.push(tmp);
    }
    else
    {
        typename VEntry::iterator it = FindEntry(parent->children_, tmp);
        it->first = BuildBoundingBox(tmp->children_);
    }
    tmp = parent;
  }
  while(!q.empty())
  {
    for (typename VEntry::iterator it = q.front()->children_.begin(); it!=q.front()->children_.end(); ++it)
    {
      InsertEntry(*it,q.front()->level_);
    }
    q.pop();
  }

}
template<typename T>
void RStarTree<T>::Delete(const BoundingBox &bounding_box, RecordType record)
{
  Node *tmp;
  tmp = FindLeaf(bounding_box,record,root_);
  if(tmp)
  {
    for (typename VEntry::iterator it = tmp->children_.begin(); it!=tmp->children_.end(); ++it)
    {
      if (bounding_box == it->first and
          record == *static_cast<RecordType *>(it->second))
        tmp->children_.erase(it);
    }
    CondenseTree(tmp);
    if(root_->children_.size() == 1) 
    {
      root_ = static_cast<Node*>(tmp->children_[0].second);
      delete(tmp);
    }
  }
  
}


#endif //RSTARTREE_RSTARTREE_H_
