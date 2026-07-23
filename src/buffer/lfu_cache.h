//
// Created by Hanhong Wong on 2026/7/21.
//

#ifndef BUSTUB_LFU_CACHE_H
#define BUSTUB_LFU_CACHE_H
#include <unordered_map>
#include "cache_policy.h"

template <typename Key, typename Value>
class LFUCache : public Cache<Key, Value> {
public:
    LFUCache(size_t capacity_) : m_capacity_(capacity_) {
    };

    LFUCache() : m_capacity_(INT_MAX) {
    }

    ~LFUCache() override {
        for (auto [freq, head] : m_freq_to_list_) {
            auto temp = head;
            while (temp != nullptr) {
                head = head->next_;
                delete temp;
                temp = head;
            }
        }
    };

    bool get(const Key& key, std::optional<Value>& value) override {
        if (contains(key) == false) {
            value = std::nullopt;
            return false;
        }

        auto node = m_key_to_node_[key];

        return putAndRefresh(node);
    }

    bool put(const Key& key, const Value& value) override {
        if (contains(key) == true) {
            auto node = m_key_to_node_[key];

            return putAndRefresh(node);
        }

        auto node = new Node<Key, Value>();
        int freq = node->freq_;
        if (m_freq_to_list_.find(freq) == m_freq_to_list_.end()) {
            auto newList = init_list();
            m_freq_to_list_[freq] = newList;
        }
        putRecent(node, freq);
        m_key_to_node_[key] = node;

        if (m_key_to_node_.size() > m_capacity_) {
            auto evictNode = evict();
            m_key_to_node_.erase(evictNode->key_);
            delete evictNode;
        }

        return true;
    }

    void setCapacity(int capacity) {
        m_capacity_ = capacity;
    }

    bool contains(const Key& key) const {
        return m_key_to_node_.find(key) != m_key_to_node_.end();
    }

public:
    using NodePtr = Node<Key, Value>*;

    NodePtr init_list() {
        auto newNode = new Node<Key, Value>();
        newNode->next_ = newNode;
        newNode->prev_ = newNode;

        return newNode;
    }

    int size() const {
        return m_key_to_node_.size();
    }

    bool empty(NodePtr node) {
        return node->next_ == node && node->prev_ == node;
    }

    bool putRecent(NodePtr node, int freq) {
        auto head = m_freq_to_list_[freq];
        node->next_ = head->next_;
        node->prev_ = head;
        head->next_->prev_ = node;
        head->next_ = node;

        return true;
    }

    std::optional<NodePtr> evict() {
        auto head = m_freq_to_list_[m_min_freq_];
        if (empty(head)) {
            return std::nullopt;
        }

        auto evictNode = head->prev_;
        removeNode(evictNode);

        return evictNode;
    }

    bool removeNode(NodePtr node) {
        auto next = node->next_;
        auto prev = node->prev_;

        next->prev_ = prev;
        prev->next_ = next;

        return true;
    }

    bool deleteNode(const Key& key) {
        auto node = m_key_to_node_[key];
        removeNode(node);
        m_key_to_node_.erase(node->key_);
        return true;
    }

    bool deleteNode(NodePtr node) {
        removeNode(node);
        m_key_to_node_.erase(node->key_);
        return true;
    }

    bool putAndRefresh(NodePtr node) {
        int oldFreq = node->freq_;
        int newFreq = node->freq_ + 1;

        removeNode(node);

        if (m_freq_to_list_.find(newFreq) != m_freq_to_list_.end()) {
            putRecent(node, newFreq);
        }
        else {
            auto newList = init_list();
            m_freq_to_list_[newFreq] = newList;
            putRecent(node, newFreq);
        }

        if (empty(m_freq_to_list_[oldFreq]) == true) {
            m_min_freq_ = newFreq;
        }
        return true;
    }

    size_t m_capacity_ = {};
    int m_min_freq_ = {};

    std::unordered_map<Key, NodePtr> m_key_to_node_ = {};
    std::unordered_map<int, NodePtr> m_freq_to_list_ = {};
};

#endif  // BUSTUB_LFU_CACHE_H
