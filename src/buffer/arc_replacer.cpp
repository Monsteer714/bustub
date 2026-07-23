// :bustub-keep-private:
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arc_replacer.cpp
//
// Identification: src/buffer/arc_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/arc_replacer.h"
#include <optional>
#include "common/config.h"
#include "lfu_cache.h"
#include "lru_cache.h"

namespace bustub {
    /**
     *
     * TODO(P1): Add implementation
     *
     * @brief a new ArcReplacer, with lists initialized to be empty and target size to 0
     * @param num_frames the maximum number of frames the ArcReplacer will be required to cache
     */
    ArcReplacer::ArcReplacer(size_t num_frames) : replacer_size_(num_frames) {
    }

    /**
     * TODO(P1): Add implementation
     *
     * @brief Performs the Replace operation as described by the writeup
     * that evicts from either mfu_ or mru_ into its corresponding ghost list
     * according to balancing policy.
     *
     * If you wish to refer to the original ARC paper, please note that there are
     * two changes in our implementation:
     * 1. When the size of mru_ equals the target size, we don't check
     * the last access as the paper did when deciding which list to evict from.
     * This is fine since the original decision is stated to be arbitrary.
     * 2. Entries that are not evictable are skipped. If all entries from the desired side
     * (mru_ / mfu_) are pinned, we instead try victimize the other side (mfu_ / mru_),
     * and move it to its corresponding ghost list (mfu_ghost_ / mru_ghost_).
     *
     * @return frame id of the evicted frame, or std::nullopt if cannot evict
     */
    auto ArcReplacer::Evict() -> std::optional<frame_id_t> {
        return std::nullopt;
    }

    /**
     * TODO(P1): Add implementation
     *
     * @brief Record access to a frame, adjusting ARC bookkeeping accordingly
     * by bring the accessed page to the front of mfu_ if it exists in any of the lists
     * or the front of mru_ if it does not.
     *
     * Performs the operations EXCEPT REPLACE described in original paper, which is
     * handled by `Evict()`.
     *
     * Consider the following four cases, handle accordingly:
     * 1. Access hits mru_ or mfu_
     * 2/3. Access hits mru_ghost_ / mfu_ghost_
     * 4. Access misses all the lists
     *
     * This routine performs all changes to the four lists as preperation
     * for `Evict()` to simply find and evict a victim into ghost lists.
     *
     * Note that frame_id is used as identifier for alive pages and
     * page_id is used as identifier for the ghost pages, since page_id is
     * the unique identifier to the page after it's dead.
     * Using page_id for alive pages should be the same since it's one to one mapping,
     * but using frame_id is slightly more intuitive.
     *
     * @param frame_id id of frame that received a new access.
     * @param page_id id of page that is mapped to the frame.
     * @param access_type type of access that was received. This parameter is only needed for
     * leaderboard tests.
     */
    void ArcReplacer::RecordAccess(frame_id_t frame_id, page_id_t page_id, [[maybe_unused]] AccessType access_type) {
        /* Page already exists in MRU/MFU:
         * This is the case where the actual cache hits. Move the page to the front of MFU. */
        if (alive_lru_.contains(frame_id) == true || alive_lfu_.contains(frame_id) == true) {
        }
        /* Page already exists in MRU ghost:
         * This is the case where the actual cache misses but we hit on the ghost list.
         * In this case we treat it as a pseudo-hit and adapt the target size.
         * If the size of the MRU ghost list is greater than or equal to the size of the MFU ghost list,
         * increase the MRU target size by one. Else increase it by MFU ghost size / MRU ghost size (rounded down).
         * Do not increase the target size above replacer_size.
         * Then move the page to the front of MFU.
         * The rational of this is if the MRU list is a little larger, then the DBMS could have had a cache hit. */
        else if (ghost_lru_.contains(frame_id) == true) {
        }
        /* Page already exists in MFU ghost:
         * Similar to the previous case, this is when the actual cache misses but we hit on the ghost list.
         * If the size of the MFU ghost list is greater than or equal to the size of the MRU ghost list,
         * decrease the MRU target size by 1. Else decrease the MRU target size by MRU ghost size / MFU ghost size (rounded down).
         * Do not decrease the target size below 0. Then move the page to the front of MFU.
         * The rational of this is if the MFU list is a little larger, the DBMS could have had a cache hit.*/
        else if (ghost_lfu_.contains(frame_id) == true) {
        }
        /* Page is not in the replacer:
         * This is the case where the actual cache misses and the ghost list misses.
         * Then either of the following should happen.
         *  If MRU size + MRU ghost size = replacer size: Kill the last element in the MRU ghost list, then add the page to the front of MRU.
         *  Else MRU size + MRU ghost size should be smaller than replacer size (it should never be larger if you do things correctly).
         *      In this case
         *      If MRU size + MRU ghost size + MFU size + MFU ghost size = 2 * replacer size:
         *      Kill the last element in the MFU ghost list, then add the page to the front of MRU.
         *      Else simply add the page to the front of the MRU.*/
        else {
            if (alive_lru_.size() + ghost_lru_.size() == replacer_size_) {
            }
            else {
                if (alive_lru_.size() + ghost_lru_.size() + alive_lfu_.size() + ghost_lfu_.size() == 2 *
                    replacer_size_) {
                }
                else {
                    FrameStatus frame_status = FrameStatus(page_id, frame_id, false, ArcStatus::MRU);
                    std::shared_ptr<FrameStatus> frame_status_ptr = std::make_shared<FrameStatus>(frame_status);
                    alive_lru_.put(frame_id, frame_status_ptr);
                }
            }
        }
    }

    /**
     * TODO(P1): Add implementation
     *
     * @brief Toggle whether a frame is evictable or non-evictable. This function also
     * controls replacer's size. Note that size is equal to number of evictable entries.
     *
     * If a frame was previously evictable and is to be set to non-evictable, then size should
     * decrement. If a frame was previously non-evictable and is to be set to evictable,
     * then size should increment.
     *
     * If frame id is invalid, throw an exception or abort the process.
     *
     * For other scenarios, this function should terminate without modifying anything.
     *
     * @param frame_id id of frame whose 'evictable' status will be modified
     * @param set_evictable whether the given frame is evictable or not
     */
    void ArcReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
        std::shared_ptr<FrameStatus> frame = std::make_shared<FrameStatus>(FrameStatus(0, 0, false, ArcStatus::MRU));

        if (contains(frame_id, frame) == false) {
            return;
        }

        bool oldEvictableStatus = frame->evictable_;
        if (set_evictable == true) {
            if (oldEvictableStatus == true) {
                return;
            }
            else if (oldEvictableStatus == false) {
                frame->evictable_ = true;
                curr_size_++;
            }
        }
        else if (set_evictable == false) {
            if (oldEvictableStatus == true) {
                frame->evictable_ = false;
                curr_size_--;
            }
            else if (oldEvictableStatus == false) {
                return;
            }
        }

        return;
    }

    /**
     * TODO(P1): Add implementation
     *
     * @brief Remove an evictable frame from replacer.
     * This function should also decrement replacer's size if removal is successful.
     *
     * Note that this is different from evicting a frame, which always remove the frame
     * decided by the ARC algorithm.
     *
     * If Remove is called on a non-evictable frame, throw an exception or abort the
     * process.
     *
     * If specified frame is not found, directly return from this function.
     *
     * @param frame_id id of frame to be removed
     */
    void ArcReplacer::Remove(frame_id_t frame_id) {
        auto frame = std::shared_ptr<FrameStatus>{};
        if (contains(frame_id, frame) == false) {
            return;
        }

        if (frame->evictable_ == false) {
            return;
        }

        if (alive_lru_.contains(frame_id) == true) {
            alive_lru_.deleteNode(frame_id);
        }
        if (alive_lfu_.contains(frame_id) == true) {
            alive_lfu_.deleteNode(frame_id);
        }

        curr_size_--;
    }

    /**
     * TODO(P1): Add implementation
     *
     * @brief Return replacer's size, which tracks the number of evictable frames.
     *
     * @return size_t
     */
    auto ArcReplacer::Size() -> size_t {
        return curr_size_;
    }
} // namespace bustub
