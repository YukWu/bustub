//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"
namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) : num_pages_(num_pages) {}

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
    if (lru_list_.empty() || evict_able_count_ == 0) {
        return false;
    }
    auto iter = lru_list_.rend();
    for (auto it = lru_list_.rbegin(); it != lru_list_.rend(); ++it) {
        if (it->evict_able) {
            iter = it;
            break; 
        }
    }
    if (iter == lru_list_.rend()) {
        return false;
    }
    *frame_id = iter->frame_id;
    lru_list_.erase((++iter).base()); // 反向迭代器的特性: https://www.cnblogs.com/kesalin/p/cpp_stl.html
    lru_map_.erase(*frame_id);
    evict_able_count_--;
    return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
    auto iter = lru_map_.find(frame_id);
    if (iter != lru_map_.end()) {
        if(iter->second->evict_able) {
            evict_able_count_--;
        }
        lru_list_.erase(iter->second);
        lru_map_.erase(iter);
        lru_list_.push_front({frame_id, false});
        lru_map_.insert({frame_id, lru_list_.begin()});
    }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
    auto iter = lru_map_.find(frame_id);
    if (iter != lru_map_.end()) {
        if (iter->second->evict_able) {
            return;
        }
        iter->second->evict_able = true;
    } else {
        lru_list_.push_front({frame_id, true});
        lru_map_.insert({frame_id, lru_list_.begin()});
    }
    evict_able_count_++;
}

auto LRUReplacer::Size() -> size_t { return evict_able_count_; }

}  // namespace bustub
