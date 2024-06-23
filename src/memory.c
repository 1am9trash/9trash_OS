#include "memory.h"
#include "uart.h"
#include "cpio.h"
#include "utils.h"

// config
#define MEMORY_START (void *)0x0
#define MEMORY_END (void *)0x3C000000
// smaller memory scope for test
// #define MEMORY_START (void *)0x1000000
// #define MEMORY_END (void *)0x1000000 + 40 * 0x1000

#define BASIC_MEMORY_START (void *)0x06000000
#define BASIC_MEMORY_END (void *)0x08000000

#define PAGE_SIZE 0x1000
#define PAGE_COUNT ((MEMORY_END - MEMORY_START) / PAGE_SIZE)

#define MAX_ORDER 12

// frame & chunk
#define FRAME_BIN_COUNT (MAX_ORDER + 1)

#define FRAME_INUSE 0x1
#define IS_INUSE(node) (node->state->flag & FRAME_INUSE)

#define CHUNK_BIN_COUNT 7

// variable
void *basic_memory_cur = BASIC_MEMORY_START;

struct FrameState *states;
struct ChunkState *c_states;
struct BuddyNode *nodes;
struct BuddyNode *frame_bins[FRAME_BIN_COUNT];
struct BuddyNode *chunk_bins[CHUNK_BIN_COUNT];

// utils
uint64_t align_to_alignment(uint64_t size, uint64_t alignment) {
    return (size + alignment - 1) & -alignment;
}

// basic alloc
void *basic_alloc(uint64_t size) {
    size = align_to_alignment(size, 8);

    if ((uint64_t)basic_memory_cur + size > (uint64_t)BASIC_MEMORY_END) {
        uart_puts("Basic memory doesn't has enough space.\n");
        return 0;
    }
    void *addr = basic_memory_cur;
    basic_memory_cur += size;

    uart_puts("Basic allocation: addr=");
    uart_hex_long((uint64_t)addr, 1);
    uart_puts(", size=");
    uart_hex_long(size, 1);
    uart_puts("\n");

    return addr;
}

// buddy system list
struct BuddyNode *insert_head(struct BuddyNode *head, struct BuddyNode *node) {
    if (head == NULL) {
        node->next = node;
        node->prev = node;
    } else {
        node->next = head;
        node->prev = head->prev;
        head->prev->next = node;
        head->prev = node;
    }
    return node;
}

struct BuddyNode *remove_head(struct BuddyNode *head) {
    if (head == head->next)
        return NULL;
    else {
        head->prev->next = head->next;
        head->next->prev = head->prev;
        return head->next;
    }
}

struct BuddyNode *remove(struct BuddyNode *head, struct BuddyNode *node) {
    if (node == node->next)
        return NULL;
    else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        return head;
    }
}

// buddy system
uint32_t addr2idx(uint64_t addr) {
    // align down to page: addr & -PAGE_SIZE
    return ((addr & -PAGE_SIZE) - (uint64_t)MEMORY_START) / PAGE_SIZE;
}

void buddy_init() {
    // frame init
    states = (struct FrameState *)basic_alloc(sizeof(struct FrameState) * PAGE_COUNT);
    for (int i = 0; i < PAGE_COUNT; i++) {
        (states + i)->flag = 0;
        (states + i)->order = 0;
    }
    for (int i = 0; i < FRAME_BIN_COUNT; i++)
        frame_bins[i] = NULL;

    // chunk init
    c_states = (struct ChunkState *)basic_alloc(sizeof(struct ChunkState) * PAGE_COUNT);
    for (int i = 0; i < PAGE_COUNT; i++) {
        (c_states + i)->order = 0;
        (c_states + i)->count = 0;
    }
    for (int i = 0; i < CHUNK_BIN_COUNT; i++)
        frame_bins[i] = NULL;

    // buddy nodes init
    nodes = (struct BuddyNode *)basic_alloc(sizeof(struct BuddyNode) * PAGE_COUNT);
    for (int i = 0; i < PAGE_COUNT; i++) {
        (nodes + i)->idx = i;
        (nodes + i)->state = (states + i);
        (nodes + i)->c_state = (c_states + i);
    }
}

void merge_buddy_frames() {
    for (int i = 0; i < MAX_ORDER; i++) {
        for (int j = 0; j < PAGE_COUNT; j += (1 << i)) {
            uint32_t idx = j;
            uint32_t buddy_idx = idx ^ (1 << i);
            struct BuddyNode *node = nodes + idx;
            struct BuddyNode *buddy_node = nodes + buddy_idx;

            if (IS_INUSE(node) || node->state->order != i)
                continue;

            if (buddy_idx < PAGE_COUNT && !IS_INUSE(buddy_node) && buddy_node->state->order == i) {
                // both are free and in same order
                node->state->order = i + 1;
                buddy_node->state->flag |= FRAME_INUSE;
                if (i + 1 == MAX_ORDER)
                    frame_bins[i + 1] = insert_head(frame_bins[i + 1], node);
            } else
                frame_bins[i] = insert_head(frame_bins[i], node);
        }
    }
}

void buddy_log() {
    uart_puts("\nBuddy System Log:\n");
    for (int i = 0; i < FRAME_BIN_COUNT; i++) {
        struct BuddyNode *cur = frame_bins[i];
        if (cur == NULL)
            continue;
        uart_puts("Order ");
        uart_hex(i, 1);
        uart_puts(": ");
        while (cur != NULL) {
            uart_hex(cur - nodes, 1);
            uart_puts(", ");
            cur = cur->next;
            if (cur == frame_bins[i])
                break;
        }
        uart_puts("\n");
    }
}

void chunk_log() {
    uart_puts("\nChunk System Log:\n");
    for (int i = 0; i < CHUNK_BIN_COUNT; i++) {
        struct BuddyNode *cur = chunk_bins[i];
        if (cur == NULL)
            continue;
        uart_puts("Order ");
        uart_hex(i, 1);
        uart_puts(":\n");
        while (cur != NULL) {
            uart_hex(cur - nodes, 1);
            uart_puts(": order=");
            uart_hex(cur->c_state->order, 1);
            uart_puts(", count=");
            uart_hex(cur->c_state->count, 1);
            uart_puts(", used=");

            uint32_t used_count = 0;
            uint32_t gap = (1 << cur->c_state->order) * 32;
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 64; k++) {
                    if (PAGE_SIZE <= gap * (j * 64 + k))
                        break;
                    if (used_count == PAGE_SIZE / gap - cur->c_state->count) {
                        uart_puts("...(remain ZEROs)");
                        break;
                    }
                    if (cur->c_state->used[j] & (1LL << k)) {
                        used_count++;
                        uart_putc('1');
                    } else
                        uart_putc('0');
                }
            }
            uart_puts("\n");
            cur = cur->next;
            if (cur == chunk_bins[i])
                break;
        }
    }
}

uint32_t page_number_to_frame_order(uint32_t page_num) {
    uint32_t order = 32;
    for (int i = 31; i >= 0; i--) {
        if ((1 << i) >= page_num)
            order = i;
        else
            break;
    }
    return order;
}

void *split_frames(uint32_t order, uint32_t target_order) {
    struct BuddyNode *node = frame_bins[order];
    frame_bins[order] = remove_head(frame_bins[order]);
    
    for (int i = order; i > target_order; i--) {
        uint32_t buddy_idx = node->idx ^ (1 << (i - 1));
        struct BuddyNode *buddy_node = nodes + buddy_idx;
        node->state->order = i - 1;
        buddy_node->state->order = i - 1;
        buddy_node->state->flag = 0;
        frame_bins[i - 1] = insert_head(frame_bins[i - 1], buddy_node);
    }

    node->state->flag |= FRAME_INUSE;
    return MEMORY_START + node->idx * PAGE_SIZE;
}

void *alloc_pages(uint32_t page_num) {
    uint32_t order = page_number_to_frame_order(page_num);
    if (order > MAX_ORDER)
        return NULL;
    for (int i = order; i <= MAX_ORDER; i++) {
        if (frame_bins[i] != NULL) 
            return split_frames(i, order);
    }
    return NULL;
}

void free_pages(void *addr) {
    if (addr < MEMORY_START || addr > MEMORY_END) {
        uart_puts("Error: Illegal memory address at ");
        uart_hex_long((uint64_t)addr, 1);
        uart_puts("\n");
        return;
    }

    uint32_t idx = (addr - MEMORY_START) / PAGE_SIZE;
    struct BuddyNode *node = nodes + idx;
    if (!IS_INUSE(node)) {
        uart_puts("Error: Double free the memory at ");
        uart_hex_long((uint64_t)addr, 1);
        uart_puts("\n");
        return;
    }

    uint32_t order = node->state->order;
    node->state->flag = 0;
    while (order < MAX_ORDER) {
        uint32_t buddy_idx = idx ^ (1 << order);
        struct BuddyNode *buddy_node = nodes + buddy_idx;
        if (buddy_idx < PAGE_COUNT && !IS_INUSE(buddy_node) && buddy_node->state->order == order) {
            // both are free and in same order
            if (buddy_idx < idx) {
                swap(&idx, &buddy_idx);
                node = nodes + idx;
                buddy_node = nodes + buddy_idx;
            }
            node->state->order = order + 1;
            frame_bins[order] = remove(frame_bins[order], buddy_node);
            buddy_node->state->flag |= FRAME_INUSE;

            // uart_hex(node->idx, 1);
            // uart_puts(" ------> merge to ");
            // uart_hex(node->state->order, 1);
            // uart_puts(" order.\n");

            if (order + 1 == MAX_ORDER)
                frame_bins[order + 1] = insert_head(frame_bins[order + 1], node);
            order++;
        } else {
            frame_bins[order] = insert_head(frame_bins[order], node);
            break;
        }
    }
}

void clean_chunk(struct ChunkState *state, uint32_t order) {
    state->order = order;
    state->count = PAGE_SIZE / ((1 << order) * 32);
    for (int i = 0; i < 2; i++)
        state->used[i] = 0;
}

void *kmalloc(uint32_t size) {
    uint32_t order = -1;
    if (size <= 32) { order = 0; size = 32; }
    else if (size <= 64) { order = 1; size = 64; }
    else if (size <= 128) { order = 2; size = 128; }
    else if (size <= 256) { order = 3; size = 256; }
    else if (size <= 512) { order = 4; size = 512; }
    else if (size <= 1024) { order = 5; size = 1024; }
    else if (size <= 2048) { order = 6; size = 2048; }
    else if (size <= 4096) { size = 4096; }

    if (order == -1)
        return alloc_pages((size - 1) / PAGE_SIZE + 1);
    
    struct BuddyNode *cur_node = chunk_bins[order];
    while (cur_node != NULL) {
        if (cur_node->c_state->count > 0)
            break;
        if (cur_node->next == chunk_bins[order]) {
            cur_node = NULL;
            break;
        }
        cur_node = cur_node->next;
    }

    if (cur_node == NULL) {
        void *ptr = alloc_pages(1);
        uint32_t idx = addr2idx((uint64_t)ptr);
        struct BuddyNode *node = nodes + idx;
        clean_chunk(node->c_state, order);
        chunk_bins[order] = insert_head(chunk_bins[order], node);
        cur_node = chunk_bins[order];
    }

    uint32_t gap = (1 << order) * 32;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            if (PAGE_SIZE <= gap * (i * 64 + j))
                break;
            if (!(cur_node->c_state->used[i] & (1LL << j))) {
                cur_node->c_state->used[i] |= (1LL << j);
                cur_node->c_state->count--;
                return MEMORY_START + cur_node->idx * PAGE_SIZE + gap * (i * 64 + j);
            }
        }
    }

    return NULL;
}

void kfree(void *addr) {
    uint32_t idx = addr2idx((uint64_t)addr);
    struct BuddyNode *node = nodes + idx;
    uint32_t gap = (1 << node->c_state->order) * 32;
    uint32_t chunk_idx = ((uint64_t)addr - (uint64_t)MEMORY_START - idx * PAGE_SIZE) / gap;

    node->c_state->count++;
    if (chunk_idx < 64)
        node->c_state->used[0] &= ~(1LL << chunk_idx);
    else
        node->c_state->used[1] &= ~(1LL << (chunk_idx - 64));
    
    if (node->c_state->count == PAGE_SIZE / gap) {
        chunk_bins[node->c_state->order] = remove(chunk_bins[node->c_state->order], node);
        free_pages(addr - chunk_idx * gap);
    }
}

void memory_reserve(uint64_t start, uint64_t end) {
    start = start & ~(PAGE_SIZE - 1);
    end = align_to_alignment(end, PAGE_SIZE);

    uart_puts("Reserve memory: ");
    uart_hex_long(start, 1);
    uart_puts(" - ");
    uart_hex_long(end, 1);
    uart_puts("\n");

    for (uint64_t i = start; i < end; i += PAGE_SIZE) {
        if (i < (uint64_t)MEMORY_START || i > (uint64_t)MEMORY_END)
            continue;
        int idx = addr2idx(i);
        (states + idx)->flag |= FRAME_INUSE;
        (states + idx)->order = 0;
    }
}

// memory system
void memory_init() {
    buddy_init();
    memory_reserve(0, 0x1000);
    memory_reserve(0x1000, 0x80000);
    memory_reserve(CPIO_BASE, CPIO_BASE + 72000);
    memory_reserve((uint64_t)BASIC_MEMORY_START, (uint64_t)BASIC_MEMORY_END);
    merge_buddy_frames();
}

// test
void test_buddy_system() {
    uart_puts("\nInitial state:");
    buddy_log();

    uint32_t test_sizes[3] = {(1 << 0), (1 << 4), (1 << 10)};
    void *ptr[3];
    for (int i = 0; i < 3; i++) {
        uart_puts("\nAlloc ");
        uart_hex(test_sizes[i], 1);
        uart_puts(" pages.");
        ptr[i] = alloc_pages(test_sizes[i]);
    }
    buddy_log();

    uart_puts("\nFree all:");
    for (int i = 0; i < 3; i++)
        free_pages(ptr[i]);
    buddy_log();
}

struct TestStruct {
    double a[6];
};

void test_dynamic_alloc() {
    uart_puts("\nInitial state:");
    chunk_log();

    uart_puts("\nAlloc 1 * int32 + 1 * int64:");
    uint32_t *a1 = kmalloc(sizeof(uint32_t));
    uint64_t *a2 = kmalloc(sizeof(uint64_t));
    chunk_log();

    uart_puts("\nAlloc 1 * struct with 6 double:");
    struct TestSruct *a3 = kmalloc(sizeof(struct TestStruct));
    chunk_log();

    uart_puts("\nAlloc 3 * 2048 byte:");
    void *a4 = kmalloc(2048);
    void *a5 = kmalloc(2048);
    void *a6 = kmalloc(2048);
    chunk_log();

    uart_puts("\nFree all:");
    kfree(a1);
    kfree(a2);
    kfree(a3);
    kfree(a4);
    kfree(a5);
    kfree(a6);
    chunk_log();
}