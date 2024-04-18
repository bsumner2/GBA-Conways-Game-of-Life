#ifndef _CONWAY_H_
#define _CONWAY_H_

#include "gba_def.h"
#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif
#define WRAP_AROUND

#define GRID_WIDTH (SCREEN_WIDTH>>1)
#define GRID_HEIGHT (SCREEN_HEIGHT>>1)
#define GRID_CELL_LEN 2
#define GRID_ROW_LAST_IDX (GRID_WIDTH-1)
#define GRID_COL_LAST_IDX  (GRID_HEIGHT-1)

#define LIVE_COLOR 0x7FFF
#define DEAD_COLOR 0x0000

#define HOVER_COLOR BLUE_MASK
#define NEUTRAL_COLOR 0x10A5
#define CURSOR_COLOR RED_MASK

#define SELECTED_COLOR GREEN_MASK

#define CLEAR_SCREEN fast_memset32(VIDEO_BUF, 0, (SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(u16_t))/sizeof(u32_t))

typedef bool_t GridRow[GRID_WIDTH];

#ifndef _CONWAY_SRC_FILE_
extern bool_t bufa[GRID_HEIGHT][GRID_WIDTH], bufb[GRID_HEIGHT][GRID_WIDTH];

extern IWRAM_CODE void fast_memset32(void *dst, u32_t val, size_t word_ct);


static const size_t grid_bytelen = sizeof(bufb);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _CONWAY_H_ */
