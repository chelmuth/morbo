/* -*- Mode: C -*- */

#include <stdlib.h>
#include <gc.h>

#include "fw_scan_memory.h"

struct memory_info *
parse_mbi_mmap(struct memory_map *mmap_buf, size_t mmap_length)
{
  struct memory_info *info_start = NULL;
  struct memory_info *info_last = NULL;
  char *cur = (char *)mmap_buf;

  while (cur < (((char *)mmap_buf) + mmap_length)) {
    struct memory_map *mmap_cur = (struct memory_map *)cur;
    struct memory_info *info = GC_MALLOC(sizeof(struct memory_info));

    if (info_start == NULL) 
      info_start = info;

    if (info_last != NULL) {
      info_last->next = info;
    }

    info_last = info;
    info->next = NULL;
    info->type = mmap_cur->type;
    info->addr = mmap_cur->base_addr_low | (uint64_t)(mmap_cur->base_addr_high)<<32;
    info->length = mmap_cur->length_low | (uint64_t)(mmap_cur->length_high)<<32;

    cur += mmap_cur->size + 4; 
  }

  return info_start;
}

bool
mem_range_available(struct memory_info *info, uint64_t start, uint64_t length)
{
  if (length == 0) return true;

  /* There must be a range of available memory enclosing
     start<->start+length. */
  for (struct memory_info *cur = info; cur != NULL; cur = cur->next) {
    
    if ((cur->type == 1) &&
	(cur->addr <= start) &&
	(cur->addr + cur->length <= start + length))
      return true;
  }

  return false;
}

/* EOF */
