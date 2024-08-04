#include "gba_types.h"
#define _CONWAY_SRC_FILE_
#include "conway.h"
EWRAM_BSS bool_t bufa[GRID_HEIGHT][GRID_WIDTH] = {0}, bufb[GRID_HEIGHT][GRID_WIDTH] = {0};

/*
void fast_memset32(void *dst, u8_t val, size_t byte_len) {
  size_t word_ct = byte_len>>2;
  u32_t word_val = (val<<24)|(val<<16)|(val<<8)|val;
  byte_len &= 3;  // Remainder
  u32_t *dst32 = (u32_t*)dst;
  u8_t *remainder = (u8_t*)(dst = (void*) (dst32+word_ct));
  while (&(*dst32++ = word_val) != dst)
    continue;
  if (byte_len&2) {
    if (byte_len&1)
      remainder[2] = val;
    *((u16_t*)dst) = word_val&0xFFFF;
    
  } else if (byte_len&1) {
    *remainder = val;
  }
}

*/

