#include "mem/mem.h"

static unsigned short memory_map[PAGING_PAGES] = {0,};

unsigned long get_free_page()
{
  for (unsigned int i = 0; i < PAGING_PAGES; i++)
  {
    if (memory_map[i] == 0)
    {
      memory_map[i] = 1;
      return LOW_MEMORY + i * PAGE_SIZE;
    }
  }
  return 0;
}

void free_page(unsigned long p)
{
  memory_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

