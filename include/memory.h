#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "stdint.h"

struct FrameState {
    uint32_t flag;
    uint32_t order;
};

struct ChunkState {
    uint32_t order;
    uint32_t count;
    uint64_t used[2];
};

struct BuddyNode {
    uint32_t idx;
    struct FrameState *state;
    struct ChunkState *c_state;
    struct BuddyNode *next, *prev;
};

// utils
uint64_t align_to_alignment(uint64_t size, uint64_t alignment);

// basic alloc
void *basic_alloc(uint64_t size);

// buddy system list
struct BuddyNode *insert_head(struct BuddyNode *head, struct BuddyNode *node);
struct BuddyNode *remove_head(struct BuddyNode *head);
struct BuddyNode *remove(struct BuddyNode *head, struct BuddyNode *node);

// buddy system
uint32_t addr2idx(uint64_t addr);
void buddy_init();
void merge_buddy_frames();
void buddy_log();
void chunk_log();
uint32_t page_number_to_frame_order(uint32_t page_num);
void *split_frames(uint32_t order, uint32_t target_order);
void *alloc_pages(uint32_t page_num);
void free_pages(void *addr);
void clean_chunk(struct ChunkState *state, uint32_t order);
void *kmalloc(uint32_t size);
void kfree(void *addr);
void memory_reserve(uint64_t start, uint64_t end);

// memory system
void memory_init();

// test
void test_buddy_system();
void test_dynamic_alloc();

#endif