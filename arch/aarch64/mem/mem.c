/**
 * @file mem.c
 * @brief Memory management implementation for the kernel.
 *
 * This file contains functions and data structures for managing memory
 * in the kernel, including paging and memory allocation.
 */
#include "mem/mem.h"

/**
 * @var memory_map
 * @brief An array representing the memory map for paging.
 *
 * This array is used to keep track of the status of each page in memory.
 * Each entry in the array corresponds to a page, and the value indicates
 * the status of that page (e.g., free, allocated).
 *
 * @note The size of the array is determined by the PAGING_PAGES macro.
 */
static unsigned short memory_map[PAGING_PAGES] = {
    0,
};

/**
 * @brief Retrieves a free memory page.
 *
 * This function searches for and returns an available free memory page.
 *
 * @return The address of the free memory page, or 0 if no free page is available.
 */
unsigned long get_free_page()
{
    for (unsigned int i = 0; i < PAGING_PAGES; i++) {
        if (memory_map[i] == 0) {
            memory_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    return 0;
}

/**
 * @brief Frees a memory page.
 *
 * This function releases the memory page at the given address.
 *
 * @param p The address of the memory page to be freed.
 */
void free_page(unsigned long p)
{
    memory_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}
