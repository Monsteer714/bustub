//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.cpp
//
// Identification: src/primer/count_min_sketch.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/count_min_sketch.h"

#include <stdexcept>
#include <string>

namespace bustub {

/**
 * Constructor for the count-min sketch.
 *
 * @param width The width of the sketch matrix.
 * @param depth The depth of the sketch matrix.
 * @throws std::invalid_argument if width or depth are zero.
 */
template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {
  /** @TODO(student) Implement this function! */
  if (width_ == 0 || depth_ == 0) {
    throw std::invalid_argument("Width or depth cannot be 0.");
  }
  sketch_ = TwoDimensionalSketch(depth_, std::vector<uint32_t>(width_, 0));

  /** @spring2026 PLEASE DO NOT MODIFY THE FOLLOWING */
  // Initialize seeded hash functions
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.push_back(this->HashFunction(i));
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(CountMinSketch &&other) noexcept : width_(other.width_), depth_(other.depth_) {
  sketch_ = std::move(other.sketch_);
  hash_functions_ = std::move(other.hash_functions_);
  pq_ = std::move(other.pq_);
}

template <typename KeyType>
auto CountMinSketch<KeyType>::operator=(CountMinSketch &&other) noexcept -> CountMinSketch & {
  this->width_ = other.width_;
  this->depth_ = other.depth_;
  sketch_ = std::move(other.sketch_);
  hash_functions_ = std::move(other.hash_functions_);
  pq_ = std::move(other.pq_);
  return *this;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Insert(const KeyType &item) {
  /** @TODO(student) Implement this function! */
  // TODO(monsteer):thread_safe

  for (uint32_t i = 0; i < depth_; i++) {
    auto index = hash_functions_[i](item) % width_;

    mutex_.lock();

    ++sketch_[i][index];

    mutex_.unlock();
  }

}

template <typename KeyType>
void CountMinSketch<KeyType>::Merge(const CountMinSketch<KeyType> &other) {
  if (width_ != other.width_ || depth_ != other.depth_) {
    throw std::invalid_argument("Incompatible CountMinSketch dimensions for merge.");
  }
  /** @TODO(student) Implement this function! */
  for (uint32_t i = 0; i < depth_; i++) {
    for (uint32_t j = 0; j < width_; j++) {
      sketch_[i][j] += other.sketch_[i][j];
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::Count(const KeyType &item) const -> uint32_t {
  uint32_t count = UINT32_MAX;
  for (uint32_t i = 0; i < depth_; i++) {
    auto index = hash_functions_[i](item) % width_;
    auto val = sketch_[i][index];
    count = std::min(count, val);
  }
  return count;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Clear() {
  /** @TODO(student) Implement this function! */
  for (auto& i : sketch_) {
    for (auto& j : i) {
      j = 0;
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::TopK(uint16_t k, const std::vector<KeyType> &candidates)
    -> std::vector<std::pair<KeyType, uint32_t>> {
  /** @TODO(student) Implement this function! */
  std::vector<KeyPair> res = {};
  for (uint32_t i = 0; i < candidates.size(); i++) {
    auto candidate = candidates[i];
    auto val = this->Count(candidate);
    pq_.emplace(std::make_pair(candidate, val));
  }

  while (!pq_.empty() && k--) {
    auto pair = pq_.top();
    pq_.pop();
    res.emplace_back(pair);
  }

  return res;
}

// Explicit instantiations for all types used in tests
template class CountMinSketch<std::string>;
template class CountMinSketch<int64_t>;  // For int64_t tests
template class CountMinSketch<int>;      // This covers both int and int32_t
}  // namespace bustub
