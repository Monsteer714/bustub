//
// Created by Hanhong Wong on 2026/7/19.
//

#ifndef BUSTUB_LRU_CACHE_H
#define BUSTUB_LRU_CACHE_H
#include <unordered_map>
#include "cache_policy.h"

template <typename Key, typename Value>
class LRUCache : public Cache<Key, Value> {
 public:
  LRUCache(size_t capacity) : m_capacity_(capacity) { init(); }

  LRUCache() : m_capacity_(INT_MAX) { init(); }

  ~LRUCache() override {
    auto temp = m_head_;
    while (temp != nullptr) {
      m_head_ = m_head_->next_;
      delete temp;
      temp = m_head_;
    }
  }

  bool get(const Key &key, std::optional<Value> &value) override {
    if (contains(key) == false) {
      value = std::nullopt;
      return false;
    }

    auto node = m_cache_[key];
    value = node->value_;
    putRecent(node);

    return true;
  }

  bool put(const Key &key, const Value &value) override {
    if (contains(key) == true) {
      auto node = m_cache_[key];
      node->value_ = value;
      putRecent(node);
      return true;
    }

    NodePtr node = new Node(key, value);
    putRecent(node);
    m_cache_[key] = node;

    if (m_capacity_ < size()) {
      evict();
    }

    return true;
  }

  void setCapacity(int capacity) { m_capacity_ = capacity; }

  bool contains(const Key &key) const { return m_cache_.find(key) != m_cache_.end(); }

 public:
  using NodePtr = Node<Key, Value> *;

  void init() {
    m_head_ = new Node<Key, Value>;
    m_tail_ = new Node<Key, Value>;

    m_head_->next_ = m_tail_;
    m_tail_->prev_ = m_head_;
  }

  size_t size() const { return m_cache_.size(); }

  bool empty() const { return m_cache_.empty(); }

  bool putRecent(NodePtr node) {
    node->prev_ = m_head_;
    node->next_ = m_head_->next_;
    m_head_->next_->prev_ = node;
    m_head_->next_ = node;

    return true;
  }

  std::optional<NodePtr> evictNode() {
    if (empty()) {
      return std::nullopt;
    }
    auto node = m_tail_->prev_;
    while (node != m_head_ && node->evictable_ == false) {
      node = node->prev_;
    }

    if (node != m_head_ && removeNode(node)) {
      return node;
    } else {
      return std::nullopt;
    }
  }

  bool removeNode(NodePtr node) {
    if (empty()) {
      return false;
    }

    auto next = node->next_;
    auto prev = node->prev_;
    prev->next_ = next;
    next->prev_ = prev;

    return true;
  }

  bool deleteNode(const Key &key) {
    auto node = m_cache_[key];
    removeNode(node);
    m_cache_.erase(node->key_);
    return true;
  }

  bool deleteNode(NodePtr node) {
    removeNode(node);
    m_cache_.erase(node->key_);
    return true;
  }

  void evict() {
    auto node = evictNode();
    m_cache_.erase((*node)->key_);
    // delete *node;
  }

  void evict(Value &value) {
    auto node = evictNode();
    m_cache_.erase((*node)->key_);

    value = (*node)->value_;
  }

  void kill() {
    auto node = evictNode();
    m_cache_.erase((*node)->key_);

    delete *node;
  }

  size_t m_capacity_ = {};
  size_t m_evictable_cnt_ = {};
  NodePtr m_head_ = {};
  NodePtr m_tail_ = {};
  std::unordered_map<Key, NodePtr> m_cache_ = {};
};

#endif  // BUSTUB_LRU_CACHE_H
