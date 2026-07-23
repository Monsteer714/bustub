//
// Created by Hanhong Wong on 2026/7/20.
//

#ifndef BUSTUB_CACHE_POLICY_H
#define BUSTUB_CACHE_POLICY_H

template <typename Key, typename Value>
struct Node {
    Node* prev_ = {};
    Node* next_ = {};
    int freq_ = {};
    Key key_ = {};
    Value value_ = {};

    Node() : key_(Key{}), value_(Value{}), prev_(nullptr), next_(nullptr), freq_(0) {
    };

    Node(const Key& key, const Value& value) : key_(key), value_(value), prev_(nullptr), next_(nullptr), freq_(0) {
    };
};

template <typename Key, typename Value>
class Cache {
public:
    virtual ~Cache() = default;

    virtual bool get(const Key& key, std::optional<Value>& value) = 0;

    virtual bool put(const Key& key, const Value& value) = 0;
};
#endif  // BUSTUB_CACHE_POLICY_H
