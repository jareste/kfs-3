#include "../utils/stdint.h"
#include "../utils/utils.h"
#include "../display/display.h"

#define PAGE_SIZE   0x1000
#define MAX_FRAMES 1024 * 1024  // Increase to support larger memory allocations

static uint8_t frame_bitmap[MAX_FRAMES / 8]; 
// 1 bit per frame, so each bit says: used(1)/free(0). 
// For 256K frames, that’s 32 KB of bitmap.

static inline void set_frame_used(uint32_t frame_number) {
    frame_bitmap[frame_number / 8] |=  (1 << (frame_number % 8));
}
static inline void set_frame_free(uint32_t frame_number) {
    frame_bitmap[frame_number / 8] &= ~(1 << (frame_number % 8));
}
static inline bool is_frame_used(uint32_t frame_number) {
    return (frame_bitmap[frame_number / 8] & (1 << (frame_number % 8))) != 0;
}

/**
 * Example PMM init:
 *   - All frames default to used
 *   - Mark only a certain region as free, or parse BIOS map to free them properly
 */
void pmm_init() {
    // Mark all frames as used by default
    for (uint32_t f = 0; f < MAX_FRAMES; f++) {
        set_frame_used(f);
    }

    // Free frames outside the identity-mapped region (assume 64MB RAM)
    uint32_t identity_end_frame = MB(16) / PAGE_SIZE; // 4096 frames (16MB)
    uint32_t end_frame = MB(128) / PAGE_SIZE; // 16384 frames (64MB)
    for (uint32_t f = identity_end_frame; f < end_frame; f++) {
        set_frame_free(f);
    }
}
/**
 * allocate_frame:
 *   Finds the first free bit in the bitmap, sets it to used,
 *   returns physical address of that frame * PAGE_SIZE.
 */
uint32_t allocate_frame()
{
    for (uint32_t frame_number = 0; frame_number < MAX_FRAMES; frame_number++)
    {
        if (!is_frame_used(frame_number))
        {
            set_frame_used(frame_number);
            return frame_number * PAGE_SIZE;  // physical addr
        }
    }

    puts_color("vmalloc: out of memory!\n", RED);
    /* Out of memory! */
    return 0;
}

void free_frame(uint32_t phys_addr)
{
    uint32_t frame_number = phys_addr / PAGE_SIZE;
    set_frame_free(frame_number);
}
