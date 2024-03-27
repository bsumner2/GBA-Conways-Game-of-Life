#include "gba.h"
#include "bmp_types.hpp"
#include "gba_def.h"
#include "gba_functions.h"
#include "gba_stdio.h"
#include "vec.hpp"

// #define _TESTING_SAVE_SLOTS_
#include "pencil_btn.h"
#include "eraser_btn.h"
#include "clr_btn.h"
#include "save_btn.h"
#include "load_btn.h"
#include "verdana.h"
#include "conway.h"
#include <cstdlib>
#include <cstring>

GridRow * cur_buf = bufa, * back_buf = bufb;
const Vec2<u16_t> cursor_dims(GRID_CELL_LEN, GRID_CELL_LEN);

#define GRID_AT(x,y) y][x

#define WAIT_RELEASE(key) do continue; while (KEY_PRESSED(key))

enum class SimState {
  RUN_SIM,
  PAUSE_NEUTRAL,
  PAUSE_DRAW,
  PAUSE_ERASE,
};

Coordinate cursor_pos = {128, 78}, prev_cursor_pos = {0,0};
typedef void (*ButtonCallback)(void*);
struct UX_Button {
  UX_Button(void) : callback(nullptr), button_texture(Mode3::BMP()), 
      button_bg(Mode3::Rect()), x(0), y(0), width(0), height(0) {
    return;
  }

  UX_Button(u16_t* button_txture_bmp, ButtonCallback cb, u16_t width, 
      u16_t height, u16_t x=0, u16_t y=0) : callback(cb), 
      button_texture(Mode3::BMP(button_txture_bmp, width, height)), 
      button_bg(Mode3::Rect(width, height, x, y)), x(x), y(y), width(width), 
      height(height) {
    return;
  }

  void Draw(u16_t color) {
    button_bg.FillDraw(color);
    button_texture.Draw(x, y);
  }

  bool_t Hovered(Coordinate cpos, Vec2<u16_t> csize) {
    if (((cpos.x + csize[0]) < x) || (cpos.x > (x + button_texture.width)))
      return false;
    if (((cpos.y + csize[1]) < y) || (cpos.y > (y + button_texture.height)))
      return false;
    return true;
  }
  
  ButtonCallback callback;

  Mode3::BMP button_texture;
  Mode3::Rect button_bg;
  u16_t x, y, width, height;
};

void unpause_redraw(bool_t erase_cursor);
void pause_menu_draw_ui(bool_t firstdraw);
void draw_cur_buf(void);


extern bool_t State_SaveGame(bool_t *cur_buf);
extern bool_t Load_SaveGame(bool_t *cur_buf);
extern void calc_prompt_len(void);
#ifdef _TESTING_SAVE_SLOTS_
typedef struct {
  char name[16];
  u8_t data[grid_bytelen];
} Save_Slot_t;
extern int Prompt_Save_Slot(Save_Slot_t *saves, int num_saves);
#endif

static void fast_memset32(void *dst, u8_t val, size_t byte_len) {
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

#define SAVE_STATUS_MESSAGE "State saved to cartridge."
#define SAVE_MSG_LEN 25
#define LOAD_STATUS_MESSAGE "State loaded from cartridge."
#define LOAD_MSG_LEN 28

static int SAVE_MESSAGE_WIDTH, LOAD_MESSAGE_WIDTH;

void swap_bufs(void);

void redraw_buf(void) {

  Mode3::Rect cell(GRID_CELL_LEN);
  bool_t* cur_row;
  for (int i = 0; i < GRID_HEIGHT; ++i) {
    cur_row = cur_buf[i];
    for (int j = 0; j < GRID_WIDTH; ++j) {
        cell.SetCoords(j*GRID_CELL_LEN, i*GRID_CELL_LEN);
        cell.FillDraw(cur_row[j]?LIVE_COLOR:DEAD_COLOR);
      }
  }

}

UX_Button 
          save_state_btn((u16_t*)save_btn, 
              [](void* arg) -> void {
                memset(VIDEO_BUF, 0, sizeof(u16_t)*SCREEN_WIDTH*SCREEN_HEIGHT);
                if (State_SaveGame((bool_t*)cur_buf)) {
                  Mode3::Rect block(SAVE_MESSAGE_WIDTH, verdana_GlyphHeight, 0, SCREEN_HEIGHT - verdana_GlyphHeight);
                  block.FillDraw(0x0C84);

                  Mode3_puts(SAVE_STATUS_MESSAGE, 0, SCREEN_HEIGHT - verdana_GlyphHeight, 0x7FFF);
                  int i = 0;
                  while (++i& 0x3F)
                    vsync();
                }
                redraw_buf();
                pause_menu_draw_ui(true);

              },save_btn_width, save_btn_height, 4, 4),
          load_state_btn((u16_t*)load_btn, 
              [](void* arg) -> void {
                memset(VIDEO_BUF, 0, sizeof(u16_t)*SCREEN_WIDTH*SCREEN_HEIGHT);
                if (Load_SaveGame((bool_t*)back_buf)) {
                  swap_bufs();
                  Mode3::Rect block(LOAD_MESSAGE_WIDTH, verdana_GlyphHeight, 0, SCREEN_HEIGHT - verdana_GlyphHeight);
                  block.FillDraw(0x0C84);
                  Mode3_puts(LOAD_STATUS_MESSAGE, 0, SCREEN_HEIGHT - verdana_GlyphHeight, 0x7FFF);
                
                  int i = 0;
                  while (++i&63)
                    vsync();
                }
                redraw_buf();
                pause_menu_draw_ui(true);
              }, load_btn_width, load_btn_height,
              4, save_state_btn.y + save_state_btn.height + 4),
          draw_btn((u16_t*)pencil_btn, 
              [](void *arg) -> void {
                *((SimState*)arg) = SimState::PAUSE_DRAW;
                unpause_redraw(false);
              }, pencil_btn_width, pencil_btn_height, 
              4, load_state_btn.y + load_state_btn.height + 4), 
          erase_btn((u16_t*)eraser_btn,
              [](void *arg) {
                *((SimState*)arg) = SimState::PAUSE_ERASE;
                unpause_redraw(false);
              }, eraser_btn_width, eraser_btn_height, 
              4, draw_btn.height + draw_btn.y + 4), 
          clear_btn((u16_t*)clr_btn, 
              [](void *arg) {
                fast_memset32(bufa, 0, sizeof(bufa)); 
                fast_memset32(bufb, 0, sizeof(bufb)); 
                fast_memset32(VIDEO_BUF, 0, 
                    sizeof(u16_t)*SCREEN_WIDTH*SCREEN_HEIGHT);
                unpause_redraw(false);
                pause_menu_draw_ui(true);
                return;
              }, clr_btn_width, clr_btn_height, 4, 
              erase_btn.y + erase_btn.height + 4);

static const int ui_btn_ct = 5;
static const UX_Button ui_buttons[ui_btn_ct] = {
  save_state_btn,
  load_state_btn,
  draw_btn,
  erase_btn,
  clear_btn
};





static const int button_ct = sizeof(ui_buttons)/sizeof(UX_Button);




STAT_INLN bool_t cursor_in_ui_button_column(Coordinate cpos) {
  return  (cpos.x + GRID_CELL_LEN >= 4 && cpos.x <= 20) 
    && (cpos.y + GRID_CELL_LEN >= 4 
    && cpos.y <= (ui_buttons[button_ct-1].y + 16));
}

int _Internal_UX_HIDX(UX_Button *a, int low, int high, Coordinate cpos, Vec2<u16_t> cdims) {
  int tmp;
  if ((tmp = high-low) < 2) {
    if (tmp)
      return a[low].Hovered(cpos, cdims) ? low : -1;
    else
      return -1;
  }

  tmp >>= 1;
  tmp += low;
  if (a[tmp].Hovered(cpos, cdims))
    return tmp;

  return cpos.y > (a[tmp].y + a[tmp].height) 
    ? _Internal_UX_HIDX(a, tmp+1, high, cpos, cdims)
    : _Internal_UX_HIDX(a, low, tmp, cpos, cdims);
}

int UIButton_HoverIdx(Coordinate cursor_pos, Vec2<u16_t> cursor_dims) {
  return _Internal_UX_HIDX((UX_Button*)(&ui_buttons[0]), 0, ui_btn_ct, cursor_pos, 
      cursor_dims);
}



/**
 * @brief Sets back buffer (back_buf) cell according to number of living
 * neighbors and current state on front buffer (cur_buf) */
static inline void back_buffer_set(int x, int y, int cnt) {
  back_buf[GRID_AT(x,y)] = cnt==3 || (cur_buf[GRID_AT(x,y)] && cnt==2);
}


void eval_corners(void) {
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

  back_buffer_set(0, 0, cnt);
  
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

  back_buffer_set(GRID_ROW_LAST_IDX, 0, cnt);
  
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
  back_buffer_set(0, GRID_COL_LAST_IDX, cnt);

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

  back_buffer_set(GRID_ROW_LAST_IDX, GRID_COL_LAST_IDX, cnt);
}

void eval_edge_rows(void) {
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
    back_buffer_set(i, 0, cnt_top);
    back_buffer_set(i, GRID_COL_LAST_IDX, cnt_btm);
  }
}

void eval_edge_columns(void) {
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
    back_buffer_set(0, i, cnt_left);
    back_buffer_set(GRID_ROW_LAST_IDX, i, cnt_right);
  }
}

void eval_body(void) {
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

      back_buffer_set(j, i, cnt);
    }
  }
}

void swap_bufs(void) {
  GridRow* new_back_buf = cur_buf;
  cur_buf = back_buf;
  back_buf = new_back_buf;
}

void step_sim(void) {
  eval_corners();
  eval_edge_rows();
  eval_edge_columns();
  eval_body();
}

void draw_cur_buf(void) {
  Mode3::Rect cell(GRID_CELL_LEN);
  bool_t* cur_row, * back_row;
  for (int i = 0; i < GRID_HEIGHT; ++i) {
    cur_row = cur_buf[i];
    back_row = back_buf[i];
    for (int j = 0; j < GRID_WIDTH; ++j)
      if (cur_row[j]^back_row[j]) {
        cell.SetCoords(j*GRID_CELL_LEN, i*GRID_CELL_LEN);
        cell.FillDraw(cur_row[j]?LIVE_COLOR:DEAD_COLOR);
      }
  }
}

void unpause_redraw(bool_t erase_cursor) {
  Mode3::Rect cell(GRID_CELL_LEN);
  if (erase_cursor) {
    cell.SetCoords(cursor_pos.x, cursor_pos.y);
    cell.FillDraw(cur_buf[GRID_AT(cursor_pos.x>>1, cursor_pos.y>>1)] 
      ? LIVE_COLOR
      : DEAD_COLOR);
  }
  int x,y;
  /*
  for (int i = 0; i < draw_btn.height; i+=2) {
    y = (i+draw_btn.y);
    for (int j = 0; j < draw_btn.width; j+=2) {
      x = (j+draw_btn.x);
      cell.SetCoords(x,y);
      cell.FillDraw(cur_buf[GRID_AT(x>>1, y>>1)]?LIVE_COLOR:DEAD_COLOR);
    }
  }
  for (int i = 0; i < erase_btn.height; i+=2) {
    y = (i+erase_btn.y);
    for (int j = 0; j < erase_btn.width; j+=2) {
      x = (j+erase_btn.x);
      cell.SetCoords(x,y);
      cell.FillDraw(cur_buf[GRID_AT(x>>1, y>>1)]?LIVE_COLOR:DEAD_COLOR);
    }
  } */
  for (int i  =0; i < 16; i+=GRID_CELL_LEN) {
    for (int j = 0; j < 16; j+=GRID_CELL_LEN) {
      x = j + 4;
      for (int k = 0; k < button_ct; ++k) {
        cell.SetCoords(x, (y=i+ui_buttons[k].y));
        cell.FillDraw(cur_buf[GRID_AT(x>>1, y>>1)]?LIVE_COLOR:DEAD_COLOR);
      }
    }
  }
  
  if (!erase_cursor) {
    cell.SetCoords(cursor_pos.x, cursor_pos.y);
    cell.FillDraw(CURSOR_COLOR);
  }
}

void handle_cursor_movement(void) {
  static int dx = 0, dy = 0;
  if (KEY_PRESSED(UP)) {
    if (KEY_PRESSED(B)) {
      cursor_pos.y -= GRID_CELL_LEN;
    } else if (dy > 0) {
        dy = 0;
    } else if (dy > -4) {
        --dy;
    } else {
        cursor_pos.y -= GRID_CELL_LEN, dy = 0;
    }
  } else if (KEY_PRESSED(DOWN)) {
    if (KEY_PRESSED(B)) {
      cursor_pos.y += GRID_CELL_LEN;
    } else if (dy < 0) {
        dy = 0;
    } else if (dy < 4) {
        ++dy;
    } else {
        cursor_pos.y += GRID_CELL_LEN, dy = 0;
    }
  }

  if (KEY_PRESSED(LEFT)) {
    if (KEY_PRESSED(B)) {
      cursor_pos.x -= GRID_CELL_LEN;
    } else if (dx > 0) {
      dx = 0;
    } else if (dx > -4) {
      --dx;
    } else {
      cursor_pos.x -= GRID_CELL_LEN, dx = 0;
    }
  } else if (KEY_PRESSED(RIGHT)) {
    if (KEY_PRESSED(B)) {
      cursor_pos.x += GRID_CELL_LEN;
    } else if (dx < 0) {
        dx = 0;
    } else if (dx < 4) {
        ++dx;
    } else {
        cursor_pos.x += GRID_CELL_LEN, dx = 0;
    }
  }
  cursor_pos.x = (cursor_pos.x < 0 
      ? 0 
      : (cursor_pos.x > (SCREEN_WIDTH - GRID_CELL_LEN) 
          ? (SCREEN_WIDTH - GRID_CELL_LEN) 
          : cursor_pos.x));
  cursor_pos.y = (cursor_pos.y < 0 
      ? 0 
      : (cursor_pos.y > (SCREEN_HEIGHT - GRID_CELL_LEN) 
          ? (SCREEN_HEIGHT - GRID_CELL_LEN)
          : cursor_pos.y));
}

bool_t handle_input(SimState& state) {
  switch (state) {
    case SimState::RUN_SIM:
      goto RUN_SIM_BRANCH;
      break;
    case SimState::PAUSE_NEUTRAL:
      goto PAUSE_MENU_BRANCH;
      break;
    case SimState::PAUSE_DRAW:
    case SimState::PAUSE_ERASE:
       goto PAUSE_MANIP_BRANCH;
      break;
  }
  return false;

RUN_SIM_BRANCH:
  if (KEY_PRESSED(START)) {
    WAIT_RELEASE(START);
    prev_cursor_pos = cursor_pos;
    state = SimState::PAUSE_NEUTRAL;

    pause_menu_draw_ui(true);
    return true;
  }
  return false;
PAUSE_MENU_BRANCH:
  prev_cursor_pos = cursor_pos;
  if (KEY_PRESSED(START)) {
    WAIT_RELEASE(START);
    state = SimState::RUN_SIM;
    unpause_redraw(true);
    return true;
  }
  
  if (KEY_PRESSED(A) && cursor_in_ui_button_column(cursor_pos)) {
    WAIT_RELEASE(A);
    int hovered_btn = 
      UIButton_HoverIdx(cursor_pos, Vec2<u16_t>(GRID_CELL_LEN, GRID_CELL_LEN));
    ui_buttons[hovered_btn].callback(&state);
    return state!=SimState::PAUSE_NEUTRAL;

  }
  handle_cursor_movement();
  return false;
PAUSE_MANIP_BRANCH:
  
  prev_cursor_pos = cursor_pos;
  
  if (KEY_PRESSED(START)) {
    WAIT_RELEASE(START);
    state = SimState::RUN_SIM;
    unpause_redraw(true);
    return true;
  } else if (KEY_PRESSED(SEL)) {
    WAIT_RELEASE(SEL);
    state = SimState::PAUSE_NEUTRAL;

    pause_menu_draw_ui(true);
    return true;
  }
  if (KEY_PRESSED(A)) {
    cur_buf[GRID_AT(cursor_pos.x>>1, cursor_pos.y>>1)] = 
      (state==SimState::PAUSE_DRAW);
  }

  handle_cursor_movement();
  return false;
}


void run_sim(SimState& state) {
  handle_input(state);
  if (state!= SimState::RUN_SIM) {
    while (KEY_PRESSED(START))
      continue;
    return;
  }
  step_sim();
  swap_bufs();
  draw_cur_buf();
}



void pause_menu_draw_ui(bool_t firstdraw) {
  static Mode3::Rect box(GRID_CELL_LEN);
  static int hovered_btn_idx = -1;
  box.SetCoords(prev_cursor_pos.x, prev_cursor_pos.y);
  box.FillDraw(cur_buf[GRID_AT(prev_cursor_pos.x>>1, prev_cursor_pos.y>>1)] 
      ? LIVE_COLOR : DEAD_COLOR);
  if (firstdraw) {
    hovered_btn_idx = cursor_in_ui_button_column(cursor_pos) ? 
      UIButton_HoverIdx(cursor_pos, cursor_dims) : -1;
    
    if (0 > hovered_btn_idx) {
      for (int i = 0; i < ui_btn_ct; ++i)
        ((UX_Button)ui_buttons[i]).Draw(NEUTRAL_COLOR);
 
      box.SetCoords(cursor_pos.x, cursor_pos.y);
      box.FillDraw(CURSOR_COLOR);
      return;
    }
    // Draw all of the buttons above the hovered one with neutral color.
    for (int i = 0; i < hovered_btn_idx; ++i)
      ((UX_Button)ui_buttons[i]).Draw(NEUTRAL_COLOR);
    
    // Draw hovered button with hovered color (duh!)
    ((UX_Button)ui_buttons[hovered_btn_idx]).Draw(HOVER_COLOR);
    
    // Draw remaining unhovered buttons, beneath hovered button, with neutral 
    // color.
    for (int i = hovered_btn_idx+1; i < ui_btn_ct; ++i)
      ((UX_Button)ui_buttons[i]).Draw(NEUTRAL_COLOR);
    
    // Finally, draw cursor last and exit.
    box.SetCoords(cursor_pos.x, cursor_pos.y);
    box.FillDraw(CURSOR_COLOR);
    return;
  }

  int newhov = cursor_in_ui_button_column(cursor_pos) ? 
      UIButton_HoverIdx(cursor_pos, cursor_dims) : -1;
  if (newhov!=hovered_btn_idx) {
    if (hovered_btn_idx > -1) {
      ((UX_Button)ui_buttons[hovered_btn_idx]).Draw(NEUTRAL_COLOR);
    } 

    if (newhov > -1) {
      ((UX_Button)ui_buttons[newhov]).Draw(HOVER_COLOR);
    }
    box.SetCoords(cursor_pos.x, cursor_pos.y);
    box.FillDraw(CURSOR_COLOR);
    hovered_btn_idx = newhov;
    return;
  }
  
  if (newhov > -1)
    ((UX_Button)ui_buttons[newhov]).Draw(HOVER_COLOR);

  box.SetCoords(cursor_pos.x, cursor_pos.y);
  box.FillDraw(CURSOR_COLOR);


#if 0
  draw_btn.Draw(draw_btn.Hovered(cursor_pos, cursor_dims)
      ? HOVER_COLOR : NEUTRAL_COLOR);
  erase_btn.Draw(erase_btn.Hovered(cursor_pos, cursor_dims)
      ? HOVER_COLOR : NEUTRAL_COLOR);
  box.SetCoords(cursor_pos.x, cursor_pos.y);
  box.FillDraw(CURSOR_COLOR);
#endif
}



void pause_menu(SimState& state) {  
  handle_input(state);
  if (state!= SimState::PAUSE_NEUTRAL) {
    switch (state) {
      case SimState::PAUSE_DRAW:
      case SimState::PAUSE_ERASE:
        while (KEY_PRESSED(A))
          continue;
        return;
      case SimState::RUN_SIM:
        while (KEY_PRESSED(START))
          continue;
        return;
      default:
        return;
    }
    return;
  }
  pause_menu_draw_ui(false);
}


void draw_buf_state(void) {
  Mode3::Rect cursor(GRID_CELL_LEN, GRID_CELL_LEN, cursor_pos.x, cursor_pos.y);
  cursor.FillDraw(CURSOR_COLOR);
  if (prev_cursor_pos.x != cursor_pos.x || prev_cursor_pos.y != cursor_pos.y) {
    cursor.SetCoords(prev_cursor_pos.x, prev_cursor_pos.y);
    cursor.FillDraw(
        cur_buf[GRID_AT(prev_cursor_pos.x>>1, prev_cursor_pos.y>>1)] 
          ? LIVE_COLOR : DEAD_COLOR);
  }
}

void draw(SimState& state) {
  handle_input(state);
  if (state != SimState::PAUSE_DRAW) {
    while (state==SimState::PAUSE_NEUTRAL?KEY_PRESSED(SEL):KEY_PRESSED(START))
      continue;
    return;
  }
  draw_buf_state();
}

void erase(SimState& state) {
  handle_input(state);
  if (state != SimState::PAUSE_ERASE)
    return;
  draw_buf_state();
}

void randomize_field(void) {
  u32_t seeda = REG_DISPLAY_VCOUNT, seedb;
  seeda ^= (REG_DISPLAY_VCOUNT<<8)^(REG_DISPLAY_VCOUNT<<4);
  seedb = REG_DISPLAY_VCOUNT<<24; 
  seedb ^= REG_DISPLAY_VCOUNT<<16^REG_DISPLAY_VCOUNT<<20;
  seeda ^= seedb^(seedb>>8)^(seeda<<8);
  srand(seeda);
  bool_t* tmp1, * tmp2;
  for (int i = 0; i < GRID_HEIGHT; ++i) {
    tmp1 = cur_buf[i], tmp2 = back_buf[i];
    for (int j = 0; j < GRID_WIDTH; ++j) {
      tmp1[j] = (rand()&1);
      tmp2[j] = false;
    }
  }
}

void calculate_msg_widths(void) {
  calc_prompt_len();
  SAVE_MESSAGE_WIDTH = LOAD_MESSAGE_WIDTH = 0;
  for (int i = 0; i < SAVE_MSG_LEN; ++i) {
    SAVE_MESSAGE_WIDTH += verdana_GlyphWidths[SAVE_STATUS_MESSAGE[i] - ' '];
  }
  for (int i = 0; i < LOAD_MSG_LEN; ++i) {
    LOAD_MESSAGE_WIDTH += verdana_GlyphWidths[LOAD_STATUS_MESSAGE[i] - ' '];
  }
}


int main(void) {
#ifndef _TESTING_SAVE_SLOTS_
  calculate_msg_widths();
  REG_DISPLAY_CNT_SET_MODES(DCNT_V_MODE3, DCNT_BG_MODE2);
  draw_cur_buf();
  SimState state = SimState::PAUSE_NEUTRAL;
  pause_menu_draw_ui(true);
  while (1) {
    vsync();
    switch (state) {
      case SimState::RUN_SIM:
        run_sim(state);
        break;
      case SimState::PAUSE_NEUTRAL:
        pause_menu(state);
        break;
      case SimState::PAUSE_DRAW:
        draw(state);
        break;
      case SimState::PAUSE_ERASE:
        erase(state);
        break;
    }
  }
#else
  REG_DISPLAY_CNT_SET_MODES(DCNT_V_MODE3, DCNT_BG_MODE2);
  calculate_msg_widths();
  Prompt_Save_Slot(nullptr, 0);
  while (1) {
    vsync();

  }
#endif
}



