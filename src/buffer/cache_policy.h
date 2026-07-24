//
// Created by Hanhong Wong on 2026/7/20.
//

#ifndef BUSTUB_CACHE_POLICY_H
#define BUSTUB_CACHE_POLICY_H
#include <optional>

template <typename Key, typename Value>
struct Node {
  Node *prev_ = {};
  Node *next_ = {};
  int freq_ = {};
  bool evictable_ = {};
  Key key_ = {};
  Value value_ = {};

  Node() : prev_(nullptr), next_(nullptr), freq_(0), evictable_(true), key_(Key{}), value_(Value{}){};

  Node(const Key &key, const Value &value)
      : prev_(nullptr), next_(nullptr), freq_(0), evictable_(true), key_(key), value_(value){};
};

template <typename Key, typename Value>
class Cache {
 public:
  virtual ~Cache() = default;

  virtual bool get(const Key &key, std::optional<Value> &value) = 0;

  virtual bool put(const Key &key, const Value &value) = 0;
};
#endif  // BUSTUB_CACHE_POLICY_H
