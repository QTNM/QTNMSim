#include <algorithm>
#include <array>
#include <queue>
#include <utility>
#include <cmath>
#include <iostream>
#include <vector>

/**
 * Class for representing a point. coordinate_type must be a numeric type.
 */
template<typename coordinate_type, size_t dimensions>
class point {
public:
  point(std::array<coordinate_type, dimensions> c) : coords_(c) {}
  point(std::initializer_list<coordinate_type> list) {
    size_t n = std::min(dimensions, list.size());
    std::copy_n(list.begin(), n, coords_.begin());
  }
  /**
   * Returns the coordinate in the given dimension.
   *
   * @param index dimension index (zero based)
   * @return coordinate in the given dimension
   */
  coordinate_type get(size_t index) const {
    return coords_[index];
  }
  /**
   * Returns the distance squared from this point to another
   * point.
   *
   * @param pt another point
   * @return distance squared from this point to the other point
   */
  double distance(const point& pt) const {
    double dist = 0;
    for (size_t i = 0; i < dimensions; ++i) {
      double d = get(i) - pt.get(i);
      dist += d * d;
    }
    return dist;
  }
private:
  std::array<coordinate_type, dimensions> coords_;
};

template<typename coordinate_type, size_t dimensions>
std::ostream& operator<<(std::ostream& out, const point<coordinate_type, dimensions>& pt) {
  out << '(';
  for (size_t i = 0; i < dimensions; ++i) {
    if (i > 0)
      out << ", ";
    out << pt.get(i);
  }
  out << ')';
  return out;
}

/**
 * C++ k-d tree implementation, based on the C version at rosettacode.org.
 */
template<typename coordinate_type, size_t dimensions>
class kdtree {
public:
  typedef point<coordinate_type, dimensions> point_type;
private:
  struct node {
    node(const point_type& pt) : point_(pt), left_(nullptr), right_(nullptr), id_(0) {}
    node(const point_type& pt, const int& idx) : point_(pt), left_(nullptr), right_(nullptr), id_(idx) {}
    coordinate_type get(size_t index) const {
      return point_.get(index);
    }
    double distance(const point_type& pt) const {
      return point_.distance(pt);
    }
    point_type point_;
    int id_; // attribute point label
    node* left_;
    node* right_;
  };
  node* root_ = nullptr;
  double best_dist_ = 0;
  std::vector<node> nodes_;
  
  struct node_cmp {
    node_cmp(size_t index) : index_(index) {}
    bool operator()(const node& n1, const node& n2) const {
      return n1.point_.get(index_) < n2.point_.get(index_);
    }
    size_t index_;
  };
  
  node* make_tree(size_t begin, size_t end, size_t index) {
    if (end <= begin)
      return nullptr;
    size_t n = begin + (end - begin)/2;
    auto i = nodes_.begin();
    std::nth_element(i + begin, i + n, i + end, node_cmp(index));
    index = (index + 1) % dimensions;
    nodes_[n].left_ = make_tree(begin, n, index);
    nodes_[n].right_ = make_tree(n + 1, end, index);
    return &nodes_[n];
  }
  
  // k-nearest neighbour booking in priority queue
  std::priority_queue<std::pair<double, node*> > pq_; // automatic order by distance double

  void k_nearest(node* root, const point_type& point, size_t index, int k) {
    if (root == nullptr)
      return;
    double d = root->distance(point);
    best_dist_ = d; // first setting
    if (pq_.size()<k || d < pq_.top().first) { // closer than biggest booked distance
      pq_.push(std::make_pair(d, root));
      if (d<best_dist_) best_dist_ = d; // closest distance booked separately
      if (pq_.size() > k) pq_.pop(); // keep k elements only
    }
    double dx = root->get(index) - point.get(index);
    index = (index + 1) % dimensions;
    k_nearest(dx > 0 ? root->left_ : root->right_, point, index, k);
    if (dx * dx >= pq_.top().first)
      return;
    k_nearest(dx > 0 ? root->right_ : root->left_, point, index, k);
  }

public:
  kdtree(const kdtree&) = delete;
  kdtree& operator=(const kdtree&) = delete;
  /**
   * Constructor taking a pair of iterators. Adds each
   * point in the range [begin, end) to the tree.
   *
   * @param begin start of range
   * @param end end of range
   */
  template<typename iterator>
  kdtree(iterator begin, iterator end) : nodes_(begin, end) {
    root_ = make_tree(0, nodes_.size(), 0);
  }
  
  /**
   * Constructor taking a function object that generates
   * points. The function object will be called n times
   * to populate the tree.
   *
   * @param f function that returns a point
   * @param n number of points to add
   */
  template<typename func>
  kdtree(func&& f, size_t n) {
    nodes_.reserve(n);
    for (size_t i = 0; i < n; ++i)
      nodes_.push_back(f());
    root_ = make_tree(0, nodes_.size(), 0);
  }
  
  /**
   * Constructor taking a vector of points.
   * Labels for nodes are being set to retrieve
   * indices for point results.
   *
   * @param vector of points
   */
  kdtree(std::vector<point_type>& data) {
    nodes_.reserve(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
      node nd(data[i], (int)i); // set an id number for point
      nodes_.push_back(nd);
    }
    root_ = make_tree(0, nodes_.size(), 0);
  }
  
  /**
   * Returns true if the tree is empty, false otherwise.
   */
  bool empty() const { return nodes_.empty(); }
  
  /**
   * Finds the k-nearest points in the tree to the given point.
   * It is not valid to call this function if the tree is empty.
   *
   * @param pt a point
   * @param k number of nearest points to find
   * @return the vector of k nearest points and indices in the tree to the given point
   */
  std::vector<std::pair<point_type, int> > k_nearest(const point_type& pt, int k) {
    if (root_ == nullptr)
      throw std::logic_error("tree is empty");

    std::vector<std::pair<point_type, int> > knn;
    best_dist_ = 0;

    k_nearest(root_, pt, 0, k); // fill bounded priority queue
    while (!pq_.empty()) { // collect points
      knn.push_back(std::make_pair(pq_.top().second->point_, pq_.top().second->id_));
      pq_.pop();
    }
    std::reverse(knn.begin(), knn.end()); // smallest distance first
    return knn;
  }

  /**
   * Finds the k-nearest points in the tree to the given point.
   * It is not valid to call this function if the tree is empty.
   *
   * @param pt a point
   * @param k number of nearest points to find
   * @return the vector of k distances and indices in the tree to the given point
   */
  std::vector<std::pair<double, int> > kn_distance_id(const point_type& pt, int k) {
    if (root_ == nullptr)
      throw std::logic_error("tree is empty");

    std::vector<std::pair<double, int> > knn;
    best_dist_ = 0;

    k_nearest(root_, pt, 0, k); // fill bounded priority queue
    while (!pq_.empty()) { // collect points
      knn.push_back(std::make_pair(pq_.top().first, pq_.top().second->id_));
      pq_.pop();
    }
    std::reverse(knn.begin(), knn.end()); // smallest distance first
    return knn;
  }
};
