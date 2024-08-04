#include "conway.h"
#include "gba_util_macros.h"

#define GRID_AT(x,y) y][x



/**
 * @brief Sets back buffer (back_buf) cell according to number of living
 * neighbors and current state on front buffer (cur_buf) */
static inline void back_buffer_set(GridRow *cur_buf, GridRow *back_buf, int x, int y, int cnt) {
  back_buf[GRID_AT(x,y)] = cnt==3 || (cur_buf[GRID_AT(x,y)] && cnt==2);
}

void eval_corners(GridRow *cur_buf, GridRow *back_buf) {
  // Top left corner (0, 0) cell
  int cnt = 0;
  for (int i = 0; i < 2; ++i) {
    if (cur_buf[GRID_AT(i, 1)])
      ++cnt;
#ifdef WRAP_AROUND
    if (cur_buf[GRID_AT(i, GRID_HEIGHT-1)])
      ++cnt;
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, i)])
      ++cnt;
#endif
    continue;
  }
#ifdef WRAP_AROUND
  if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, GRID_HEIGHT-1)])
    ++cnt;
#endif
  if (cur_buf[GRID_AT(1, 0)])
    ++cnt;

  back_buffer_set(cur_buf, back_buf, 0, 0, cnt);
  
  // Top right corner (GRID_ROW_LAST_IDX, 0) cell
  cnt = 0;
/*  for (int i = GRID_ROW_LAST_IDX-1; i < GRID_WIDTH; ++i) {
    if (cur_buf[GRID_AT(i, 1)])
      ++cnt;
    continue;
  }*/
  for (int i = 0; i < 2; ++i) {
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-i, 1)])
      ++cnt;
#ifdef WRAP_AROUND
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-i, GRID_HEIGHT-1)])
      ++cnt;
    if (cur_buf[GRID_AT(0, i)])
      ++cnt;
#endif
    continue;
  }
#ifdef WRAP_AROUND
  if (cur_buf[GRID_AT(0, GRID_HEIGHT-1)])
    ++cnt;
#endif
  if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-1, 0)])
    ++cnt;

  back_buffer_set(cur_buf, back_buf, GRID_ROW_LAST_IDX, 0, cnt);
  
  // Bottom left corner (0, GRID_COL_LAST_IDX) cell
  cnt = 0;
  for (int i = 0; i < 2; ++i) {
    if (cur_buf[GRID_AT(i, GRID_COL_LAST_IDX-1)])
      ++cnt;
#ifdef WRAP_AROUND
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, GRID_COL_LAST_IDX-i)])
      ++cnt;
    if (cur_buf[GRID_AT(i, 0)])
      ++cnt;
#endif
    continue;
  }
#ifdef WRAP_AROUND
  if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, 0)])
    ++cnt;
#endif
  if (cur_buf[GRID_AT(1, GRID_COL_LAST_IDX)])
    ++cnt;
  back_buffer_set(cur_buf, back_buf, 0, GRID_COL_LAST_IDX, cnt);

  // Bottom right corner (GRID_ROW_LAST_IDX, GRID_COL_LAST_IDX) cell
  cnt = 0;
  /*for (int i = GRID_ROW_LAST_IDX-1; i < GRID_WIDTH; ++i) {
    if (cur_buf[GRID_AT(i, GRID_COL_LAST_IDX-1)])
      ++cnt;
    continue;
  }*/
  for (int i = 0; i < 2; ++i) {
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-i, GRID_COL_LAST_IDX-1)])
      ++cnt;
#ifdef WRAP_AROUND
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-i, 0)])
      ++cnt;
    if (cur_buf[GRID_AT(0, GRID_COL_LAST_IDX-i)])
      ++cnt;
#endif
    continue;
  }
#ifdef WRAP_AROUND
  if (cur_buf[GRID_AT(0,0)])
    ++cnt;
#endif
  
  if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-1, GRID_COL_LAST_IDX)])
    ++cnt;

  back_buffer_set(cur_buf, back_buf, GRID_ROW_LAST_IDX, GRID_COL_LAST_IDX, cnt);
}



IWRAM_CODE void eval_edge_columns(GridRow *cur_buf, GridRow *back_buf) {
  int cnt_left, cnt_right, i, j;
  for (i = 1; i < GRID_COL_LAST_IDX; ++i) {
    cnt_left = cnt_right = 0;
    for (j = -1; j < 2; ++j) {
      if (cur_buf[GRID_AT(1, i+j)])
        ++cnt_left;
      if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX-1, i+j)])
        ++cnt_right;
#ifdef WRAP_AROUND
      if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, i+j)])
        ++cnt_left;
      if (cur_buf[GRID_AT(0, i+j)])
        ++cnt_right;
#endif
    }
    if (cur_buf[GRID_AT(0, i-1)])
      ++cnt_left;
    if (cur_buf[GRID_AT(0, i+1)])
      ++cnt_left;

    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, i-1)])
      ++cnt_right;
    if (cur_buf[GRID_AT(GRID_ROW_LAST_IDX, i+1)])
      ++cnt_right;
    back_buffer_set(cur_buf, back_buf, 0, i, cnt_left);
    back_buffer_set(cur_buf, back_buf, GRID_ROW_LAST_IDX, i, cnt_right);
  }
}

IWRAM_CODE void eval_body(GridRow *cur_buf, GridRow *back_buf) {
  int i, j, cnt;
  for (i = 1; i < GRID_COL_LAST_IDX; ++i) {
    for (j = 1; j < GRID_ROW_LAST_IDX; ++j) {
      cnt = 0;
      for (int k = -1; k < 2; ++k) {
        if (cur_buf[GRID_AT(j+k, i-1)])
          ++cnt;
        if (cur_buf[GRID_AT(j+k, i+1)])
          ++cnt;
      }
      if (cur_buf[GRID_AT(j-1, i)])
        ++cnt;
      if (cur_buf[GRID_AT(j+1, i)])
        ++cnt;

      back_buffer_set(cur_buf, back_buf, j, i, cnt);
    }
  }
}

IWRAM_CODE void eval_edge_rows(GridRow *cur_buf, GridRow *back_buf) {
  int cnt_top, cnt_btm, i, j;
  for (i = 1; i < GRID_ROW_LAST_IDX; ++i) {
    cnt_top = cnt_btm = 0;
    for (j = -1; j < 2; ++j) {
      if (cur_buf[GRID_AT(i+j, 1)])
        ++cnt_top;
      if (cur_buf[GRID_AT(i+j, GRID_COL_LAST_IDX-1)])
        ++cnt_btm;
#ifdef WRAP_AROUND
      if (cur_buf[GRID_AT(i+j, GRID_COL_LAST_IDX)])
        ++cnt_top;
      if (cur_buf[GRID_AT(i+j, 0)])
        ++cnt_btm;
#endif
    }
    if (cur_buf[GRID_AT(i-1, 0)])
      ++cnt_top;
    if (cur_buf[GRID_AT(i+1, 0)])
      ++cnt_top;

    if (cur_buf[GRID_AT(i-1, GRID_COL_LAST_IDX)])
      ++cnt_btm;
    if (cur_buf[GRID_AT(i+1, GRID_COL_LAST_IDX)])
      ++cnt_btm;
    back_buffer_set(cur_buf, back_buf, i, 0, cnt_top);
    back_buffer_set(cur_buf, back_buf, i, GRID_COL_LAST_IDX, cnt_btm);
  }
}


IWRAM_CODE void step_sim(GridRow *cur_buf, GridRow *back_buf) {
  eval_corners(cur_buf, back_buf);
  eval_edge_rows(cur_buf, back_buf);
  eval_edge_columns(cur_buf, back_buf);
  eval_body(cur_buf, back_buf);
}
