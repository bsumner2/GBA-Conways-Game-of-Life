#include "bmp_types.hpp"
#include "gba_bitfield_macros.h"
#include "gba_def.h"
#include "gba_functions.h"
#include "gba_inlines.h"
#include "gba_stdio.h"
#include "conway.h"
#include "gba_util_macros.h"
#include "burtana.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>


static const int GRID_BITPACK_ROWLEN = GRID_WIDTH/8;

typedef struct {
  char slot_name[16];
  u8_t grid_state[GRID_BITPACK_ROWLEN*GRID_HEIGHT];
} Save_Slot_t;


#define MAX_SAVES (((1<<16)-5)/sizeof(Save_Slot_t))


EWRAM_BSS Save_Slot_t _g_saves[MAX_SAVES];


#define SAVE_SLOTS_PER_PAGE 1
#define SLOT_DISPLAY_WIDTH 136
#define SLOT_BORDER_LEN 2
#define SLOT_TEXT_LEFT_PAD 2
#define SLOT_DISPLAY_HEIGHT burtana_CellHeight
static const int SLOT_X_ALIGNMENT = ((SCREEN_WIDTH-128)>>1);
static const int SLOT_Y_ALIGNMENT = ((SCREEN_HEIGHT - (SAVE_SLOTS_PER_PAGE+1)*SLOT_DISPLAY_HEIGHT)>>1);
#define PROMPT_BG 0x10A5
#define SLOT_BG 0x1840
#define SLOT_SEL_BG 0x2060
#define SLOT_BORDER 0x3CE0
#define SLOT_SEL_BORDER 0x6580
#define SLOT_FG 0x5672
#define SLOT_SEL_FG 0x7BDE

#define NEW_SAVE_TXT "[ + ] New Save Slot"

#define PROMPT_DIRECTIONS "B: Cancel A: Confirm\nL,R: Navigate Saves Select: Preview"
#define PROMPT_DIRECTIONS_WITH_COLOR_CODES "\x1b[0x001F]B: Cancel \x1b[0x03E0]A: Confirm\n\x1b[0x6371]L,R: Navigate Saves \x1b[0x7DA5]Select: Preview"
#define PROMPT_DIRECTIONS_MSG_LEN 46


static int prompt_directions_message_width = 0;
static int prompt_directions_message_height = 0;

void Draw_keyboard(int idx, bool_t caps_lock) {
  static Mode3::Rect charbox(burtana_CellWidth+2, burtana_GlyphHeight+2, 0, 18);
  int upper_row_startx = (SCREEN_WIDTH - 8*charbox.width)>>1,
      lower_row_startx = (SCREEN_WIDTH -10*charbox.width)>>1, curr = 0, ofs = 'a';
  if (caps_lock)
    ofs = 'A';
  for (int i = 0; i < 8; ++i) {
    charbox.x = upper_row_startx + i*charbox.width;
    for (int j = 0; j < 2; ++j) {
      charbox.y = 18 + j*charbox.height;
      if ((curr = j*8+i) == idx) {
        charbox.FillDraw(SLOT_SEL_BG);
        charbox.OutlineDraw(1, SLOT_SEL_BORDER);
        Mode3_putchar(charbox.x+1, charbox.y+1, ofs + curr, SLOT_SEL_FG);
      } else {
        charbox.FillDraw(SLOT_BG);
        charbox.OutlineDraw(1, SLOT_BORDER);
        Mode3_putchar(charbox.x+1, charbox.y+1, ofs + curr, SLOT_FG);
      }
    }
  }

  charbox.y = 18 + 2*charbox.height;
  for (int i = 16; i < 26; ++i) {
    charbox.x = lower_row_startx + (i-16)*charbox.width;
    if (idx == i) {
      charbox.FillDraw(SLOT_SEL_BG);
      charbox.OutlineDraw(1, SLOT_SEL_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + i, SLOT_SEL_FG);
    } else {
      charbox.FillDraw(SLOT_BG);
      charbox.OutlineDraw(1, SLOT_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + i, SLOT_FG);
    }
  }
  
  charbox.x = (SCREEN_WIDTH - charbox.width)>>1;
  charbox.y += charbox.height;
  if (idx == 26) {
    charbox.FillDraw(SLOT_SEL_BG);
    charbox.OutlineDraw(1, SLOT_SEL_BORDER);
    Mode3_putchar(charbox.x+1, charbox.y+1, '_', SLOT_SEL_FG);
  } else {
    charbox.FillDraw(SLOT_BG);
    charbox.OutlineDraw(1, SLOT_BORDER);
    Mode3_putchar(charbox.x+1, charbox.y+1, '_', SLOT_FG);
  }
}


void update_keyboard(int prev_hl, int new_hl, bool_t caps) {
  static Mode3::Rect charbox(burtana_CellWidth+2, burtana_GlyphHeight+2, 0, 18);
  int upper_row_startx = (SCREEN_WIDTH - 8*charbox.width)>>1,
      lower_row_startx = (SCREEN_WIDTH -10*charbox.width)>>1, ofs = 'a';
  if (caps)
    ofs = 'A';
  int starty = 18;
    


  if (prev_hl >= 0) {
    if (prev_hl  == 26) {
      charbox.x = (SCREEN_WIDTH-charbox.width)>>1;
      charbox.y = starty + charbox.height*3;
      charbox.FillDraw(SLOT_BG);
      charbox.OutlineDraw(1, SLOT_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, '_', SLOT_FG);


    } else if (prev_hl > 15) {
      charbox.x = lower_row_startx + (prev_hl - 16)*charbox.width;
      charbox.y = starty + charbox.height*2;
      charbox.FillDraw(SLOT_BG);
      charbox.OutlineDraw(1, SLOT_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + prev_hl, SLOT_FG);
    } else if (prev_hl > 7) {
      charbox.x = upper_row_startx +(prev_hl - 8)*charbox.width;
      charbox.y = starty + charbox.height;
      charbox.FillDraw(SLOT_BG);
      charbox.OutlineDraw(1, SLOT_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + prev_hl, SLOT_FG);
    } else {
      charbox.x = upper_row_startx + prev_hl*charbox.width;
      charbox.y = starty;
      charbox.FillDraw(SLOT_BG);
      charbox.OutlineDraw(1, SLOT_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + prev_hl, SLOT_FG);
    }
  }

  if (new_hl >= 0) {
      if (new_hl  == 26) {
      charbox.x = (SCREEN_WIDTH-charbox.width)>>1;
      charbox.y = starty + charbox.height*3;
      charbox.FillDraw(SLOT_SEL_BG);
      charbox.OutlineDraw(1, SLOT_SEL_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, '_', SLOT_SEL_FG);


    } else if (new_hl > 15) {
      charbox.x = lower_row_startx + (new_hl - 16)*charbox.width;
      charbox.y = starty + charbox.height*2;
      charbox.FillDraw(SLOT_SEL_BG);
      charbox.OutlineDraw(1, SLOT_SEL_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + new_hl, SLOT_SEL_FG);
    } else if (new_hl > 7) {
      charbox.x = upper_row_startx +(new_hl - 8)*charbox.width;
      charbox.y = starty + charbox.height;
      charbox.FillDraw(SLOT_SEL_BG);
      charbox.OutlineDraw(1, SLOT_SEL_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + new_hl, SLOT_SEL_FG);
    } else {
      charbox.x = upper_row_startx + new_hl*charbox.width;
      charbox.y = starty;
      charbox.FillDraw(SLOT_SEL_BG);
      charbox.OutlineDraw(1, SLOT_SEL_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, ofs + new_hl, SLOT_SEL_FG);
    }
  }
}



#define INPUT_BOX_BG 0x1441
#define INPUT_BOX_BORDER 0x2083
#define INPUT_BOX_FG 0x5F0B
#define INPUT_SEL_BG 0x2083
#define INPUT_SEL_BORDER 0x3547
#define INPUT_SEL_FG 0x77ED

int Mode3_gets(char *buf, int maxlen) {
  Mode3::Rect charbox(burtana_CellWidth+2, 2+burtana_GlyphHeight, (SCREEN_WIDTH - (burtana_CellWidth+2)*maxlen)>>1, 0), 
    caps(26+2, 2+burtana_GlyphHeight, SCREEN_WIDTH-2-26-2, 18), backspace(10+2, 2+burtana_GlyphHeight, caps.x, caps.y + caps.height);
  int inbox_startx = charbox.x, inbox_y = charbox.y;
  int inbox_cursor = 0, kbd_cursor = 0, prev;
  static const int space_idx = 26, caps_idx = 27, backspace_idx = 28;
  bool_t caps_lock = true;
  if (*buf)
    while ('\0' != buf[++inbox_cursor]);

  if (!inbox_cursor) {
    Mode3::Rect inbox(charbox.width*maxlen, charbox.height, charbox.x, charbox.y);
    inbox.FillDraw(INPUT_BOX_BG);
    charbox.FillDraw(INPUT_SEL_BG);
    charbox.OutlineDraw(1, INPUT_SEL_BORDER);
    for (int i = 1; i < maxlen; ++i) {
      charbox.x += charbox.width;
      charbox.FillDraw(INPUT_BOX_BG);
      charbox.OutlineDraw(1, INPUT_BOX_BORDER);
    }
    
    charbox.x = inbox.x;
    
  } else {
    Mode3::Rect inbox(charbox.width*maxlen, charbox.height, charbox.x, charbox.y);
    int i;
    inbox.FillDraw(INPUT_BOX_BG);
    for (i=0; i < inbox_cursor; ++i) {
      charbox.FillDraw(INPUT_BOX_BG);
      charbox.OutlineDraw(1, INPUT_BOX_BORDER);
      Mode3_putchar(charbox.x+1, charbox.y+1, buf[i], INPUT_BOX_FG);
      charbox.x += charbox.width;
    }
    if (inbox_cursor < maxlen) {
      charbox.FillDraw(INPUT_SEL_BG);
      charbox.OutlineDraw(1, INPUT_SEL_BORDER);
      for (i = inbox_cursor+1; i < maxlen; ++i) {
        charbox.x += charbox.width;
        charbox.FillDraw(INPUT_BOX_BG);
        charbox.OutlineDraw(1, INPUT_BOX_BORDER);
      }
    }
    
    charbox.x = inbox.x;


  }

  
  Draw_keyboard(kbd_cursor, caps_lock);
  
  caps.FillDraw(SLOT_BG);
  caps.OutlineDraw(1, SLOT_BORDER);
  Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_FG);
  
  backspace.FillDraw(SLOT_BG);
  backspace.OutlineDraw(1, SLOT_BORDER);
  Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_FG);

  vsync();
  Mode3_printf(0, SCREEN_HEIGHT - burtana_GlyphHeight, 0x7FFF, "\x1b[0x001F][B]: Cancel Save  \x1b[0x03E0][Start]: Confirm Save");
  while (1) {
    vsync();
    
    if (KEY_PRESSED(START)) {
      DEBOUNCE_KEY(START);
      if (inbox_cursor > 0)
        return inbox_cursor;
    } else if (KEY_PRESSED(B)) {
      DEBOUNCE_KEY(B);
      return -1;
    }


    if (KEY_PRESSED(A)) {
      DEBOUNCE_KEY(A);
      switch (kbd_cursor) {
        case caps_idx:
          caps_lock = !caps_lock;
          Draw_keyboard(-1, caps_lock);
          continue;
          break;
        case backspace_idx:
          if (inbox_cursor == 0)
            continue;
          if (inbox_cursor < maxlen) {
            charbox.x = inbox_startx + charbox.width * inbox_cursor--;
            charbox.FillDraw(INPUT_BOX_BG);
            charbox.OutlineDraw(1, INPUT_BOX_BORDER);
            charbox.x -= charbox.width;
          } else {
            charbox.x = inbox_startx + charbox.width * --inbox_cursor;
          }
          charbox.FillDraw(INPUT_SEL_BG);
          charbox.OutlineDraw(1, INPUT_SEL_BORDER);
          buf[inbox_cursor] = '\0';
          continue;
          break;
        default:
          if (++inbox_cursor > maxlen) {
            inbox_cursor = maxlen;
            continue;
          }
          charbox.x = inbox_startx + charbox.width*(inbox_cursor-1);
          charbox.FillDraw(INPUT_BOX_BG);
          charbox.OutlineDraw(1, INPUT_BOX_BORDER);
          if (kbd_cursor != space_idx)
            buf[inbox_cursor-1] = (caps_lock ? 'A' : 'a') + kbd_cursor;
          else
            buf[inbox_cursor-1] = ' ';
          Mode3_putchar(charbox.x+1, charbox.y+1, buf[inbox_cursor-1], INPUT_BOX_FG);
          if (inbox_cursor == maxlen)
            continue;
          charbox.x += charbox.width;
          charbox.FillDraw(INPUT_SEL_BG);
          charbox.OutlineDraw(1, INPUT_SEL_BORDER);
          continue;
          break;
      }
      continue;
    }

    if (KEY_PRESSED(UP)) {
      DEBOUNCE_KEY(UP);
      if (kbd_cursor > 25) {
        switch (kbd_cursor) {
          case space_idx:
            kbd_cursor = 21;
            update_keyboard(space_idx, kbd_cursor, caps_lock);
            break;
          case caps_idx:
            kbd_cursor = backspace_idx;

            caps.FillDraw(SLOT_BG);
            caps.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_FG);

            backspace.FillDraw(SLOT_SEL_BG);
            backspace.OutlineDraw(1, SLOT_SEL_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_SEL_FG);
            break;
          case backspace_idx:
            kbd_cursor = caps_idx;

            caps.FillDraw(SLOT_SEL_BG);
            caps.OutlineDraw(1, SLOT_SEL_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_SEL_FG);

            backspace.FillDraw(SLOT_BG);
            backspace.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_FG);
            break;
          default:
            break;
        }
        continue;
      }
      
      prev = kbd_cursor;

      if (kbd_cursor > 15) {
        if (kbd_cursor == 16) {
          kbd_cursor = 8;
        } else if (kbd_cursor == 25) {
          kbd_cursor = 15;
        } else {
          kbd_cursor -= 9;
        }
      } else if (kbd_cursor > 7) {
        kbd_cursor -= 8;
      } else {
        kbd_cursor = space_idx;
      }
      update_keyboard(prev, kbd_cursor, caps_lock);
      continue;
    } else if (KEY_PRESSED(DOWN)) {
      DEBOUNCE_KEY(DOWN);
      if (kbd_cursor > 25) {
        switch (kbd_cursor) {
          case space_idx:
            kbd_cursor = 4;
            update_keyboard(space_idx, kbd_cursor, caps_lock);
            break;
          case caps_idx:
            kbd_cursor = backspace_idx;

            caps.FillDraw(SLOT_BG);
            caps.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_FG);

            backspace.FillDraw(SLOT_SEL_BG);
            backspace.OutlineDraw(1, SLOT_SEL_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_SEL_FG);
            break;
          case backspace_idx:
            kbd_cursor = caps_idx;

            caps.FillDraw(SLOT_SEL_BG);
            caps.OutlineDraw(1, SLOT_SEL_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_SEL_FG);

            backspace.FillDraw(SLOT_BG);
            backspace.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_FG);
            break;
          default:
            break;
        }
        continue;
      }

      prev = kbd_cursor;
      if (kbd_cursor > 15) {
        kbd_cursor = space_idx;
      } else if (kbd_cursor > 7) {
        kbd_cursor += 9;
      } else {
        kbd_cursor += 8;
      }
      update_keyboard(prev, kbd_cursor, caps_lock);
      continue;
    }


    if (KEY_PRESSED(LEFT)) {
      DEBOUNCE_KEY(LEFT);
      if (kbd_cursor > 25) {
        switch (kbd_cursor) {
          case space_idx:
            break;
          case caps_idx:
            kbd_cursor = 7;
            caps.FillDraw(SLOT_BG);
            caps.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_FG);
            update_keyboard(-1, kbd_cursor, caps_lock);
            break;
          case backspace_idx:
            kbd_cursor = 15;
            backspace.FillDraw(SLOT_BG);
            backspace.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_FG);
            update_keyboard(-1, kbd_cursor, caps_lock);
            break;
          default:
            break;
        }
        continue;
      }
      prev = kbd_cursor;
      if (kbd_cursor > 15) {
        if (kbd_cursor == 16)
          kbd_cursor = 25;
        else
          --kbd_cursor;
        update_keyboard(prev, kbd_cursor, caps_lock);
        continue;
      } else if (kbd_cursor > 7) {
        if (kbd_cursor == 8) {
          kbd_cursor = backspace_idx;
          backspace.FillDraw(SLOT_SEL_BG);
          backspace.OutlineDraw(1, SLOT_SEL_BORDER);
          Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_SEL_FG);
          update_keyboard(prev, -1, caps_lock);
        } else {
          --kbd_cursor;
          update_keyboard(prev, kbd_cursor, caps_lock);
        }
        continue;
      } else {
        if (!kbd_cursor) {
          kbd_cursor = caps_idx;
          caps.FillDraw(SLOT_SEL_BG);
          caps.OutlineDraw(1, SLOT_SEL_BORDER);
          Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_SEL_FG);
          update_keyboard(prev, -1, caps_lock);

        } else {
          --kbd_cursor;
          update_keyboard(prev, kbd_cursor, caps_lock);
        }
        continue;
      }

    } else if (KEY_PRESSED(RIGHT)) {
      DEBOUNCE_KEY(RIGHT);
      if (kbd_cursor > 25) {
        switch (kbd_cursor) {
          case space_idx:
            break;
          case caps_idx:
            kbd_cursor = 0;
            caps.FillDraw(SLOT_BG);
            caps.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_FG);
            update_keyboard(-1, kbd_cursor, caps_lock);
            break;
          case backspace_idx:
            kbd_cursor = 8;
            backspace.FillDraw(SLOT_BG);
            backspace.OutlineDraw(1, SLOT_BORDER);
            Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_FG);
            update_keyboard(-1, kbd_cursor, caps_lock);
            break;
          default:
            break;
        }
        continue;
      }
      prev = kbd_cursor;
      if (kbd_cursor > 15) {
        if (kbd_cursor == 25)
          kbd_cursor = 16;
        else
          ++kbd_cursor;

        update_keyboard(prev, kbd_cursor, caps_lock);
        continue;
      } else if (kbd_cursor > 7) {
        if (kbd_cursor == 15) {
          kbd_cursor = backspace_idx;
          backspace.FillDraw(SLOT_SEL_BG);
          backspace.OutlineDraw(1, SLOT_SEL_BORDER);
          Mode3_puts("<-", backspace.x+1, backspace.y+1, SLOT_SEL_FG);

          update_keyboard(prev, -1, caps_lock);
        } else {
          ++kbd_cursor;
          update_keyboard(prev, kbd_cursor, caps_lock);
        }
        continue;
      } else {
        if (kbd_cursor == 7) {
          kbd_cursor = caps_idx;
          caps.FillDraw(SLOT_SEL_BG);
          caps.OutlineDraw(1, SLOT_SEL_BORDER);
          Mode3_puts("CAPS", caps.x+1, caps.y+1, SLOT_SEL_FG);
          update_keyboard(prev, -1, caps_lock);

        } else {
          ++kbd_cursor;
          update_keyboard(prev, kbd_cursor, caps_lock);
        }
        continue;
      }



    }
  }
  



}

void calc_prompt_len(void) {
  const char p[] = PROMPT_DIRECTIONS;
  const char *cursor = p;
  int idx, tmp = 0;
  while ((idx = *cursor++) != '\n') {
    idx -= ' ';
    tmp += burtana_GlyphWidths[idx];
  }
  
  while ((idx = *cursor++)) {
    idx -= ' ';
    prompt_directions_message_width += burtana_GlyphWidths[idx];
  }
  
  prompt_directions_message_width = tmp > prompt_directions_message_width ? tmp : prompt_directions_message_width;
  prompt_directions_message_height = burtana_GlyphHeight*2;

  
}






void Display_Preview(const Save_Slot_t *save, int slot_no) {
  {  // Preview Loading
    int startx = (SCREEN_WIDTH-GRID_WIDTH)>>1, starty = (SCREEN_HEIGHT-GRID_HEIGHT)>>1;
    u16_t *vram_line;

    const u8_t *grid_state = save->grid_state, *save_line;
    u8_t curr_byte;
     
    for (int i = 0; i < GRID_HEIGHT; ++i) {
      vram_line = VIDEO_BUF + SCREEN_WIDTH*(starty+i) + startx;
      save_line = grid_state + GRID_BITPACK_ROWLEN*i;
      for (int j = 0; j < GRID_BITPACK_ROWLEN; ++j) {
        curr_byte = *save_line++;
        for (int k = 0; k < 8; ++k) {
          *vram_line++ = (curr_byte&(1<<k))?0x7FFF:0;
        }
      }
    }
  }
  vsync();
  Mode3_printf(0, 0, 0x7fff, "Previewing Slot no. %d: %s\n\x1b[0x001F]B:\x1b[0x7FFF] Exit Preview", slot_no, save->slot_name);
  while (!KEY_PRESSED(B))
    continue;
  DEBOUNCE_KEY(B);
  vsync();
  // pseudo-erase
  Mode3_printf(0, 0, 0, "Previewing Slot no. %d: %s\nB: Exit Preview", slot_no, save->slot_name);
  vsync();
  Mode3::Rect cover(GRID_WIDTH, GRID_HEIGHT, (SCREEN_WIDTH-GRID_WIDTH)>>1, (SCREEN_HEIGHT-GRID_HEIGHT)>>1);
  cover.FillDraw(0);
  vsync();



}

void Draw_Save_Prompt_Directions(void) {
  Mode3::Rect prompt_square(prompt_directions_message_width, prompt_directions_message_height, 
      SCREEN_WIDTH - prompt_directions_message_width, SCREEN_HEIGHT - prompt_directions_message_height);
  prompt_square.FillDraw(PROMPT_BG);
  Mode3_printf(SCREEN_WIDTH - prompt_directions_message_width, 
      SCREEN_HEIGHT - prompt_directions_message_height, 0x7fff, PROMPT_DIRECTIONS_WITH_COLOR_CODES);
}

int Prompt_Load_Slot(Save_Slot_t *saves, int num_saves) {
  Mode3::Rect selection_square(SLOT_DISPLAY_WIDTH, SLOT_DISPLAY_HEIGHT, SLOT_X_ALIGNMENT, SLOT_Y_ALIGNMENT);
  int curr = 0;
  bool_t skip_nav_button_check = false;
  Draw_Save_Prompt_Directions();
  selection_square.FillDraw(SLOT_SEL_BG);
  selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_SEL_BORDER);
  Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
      selection_square.y + SLOT_BORDER_LEN, SLOT_SEL_FG, "%d) %s", curr+1, saves[curr].slot_name);
  
  Mode3_printf(0, 0, (num_saves == MAX_SAVES) ? 0x001F : (num_saves > MAX_SAVES>>1) ? 0x02DF : 0x03E0, "[ %d / %d ] \x1b[0x7FFF]Slots Used", num_saves, MAX_SAVES);
  
  
  while (1) {
    vsync();
    if (KEY_PRESSED(A)) {
      DEBOUNCE_KEY(A);
      return curr;
    } else if (KEY_PRESSED(B)) {
      DEBOUNCE_KEY(B);
      return -1;
    } else if (KEY_PRESSED(SEL)) {
      DEBOUNCE_KEY(SEL);
      

      // Draw over slots text with black.
      Mode3_printf(0, 0, 0, "[ %d / %d ] Slots Used", num_saves, MAX_SAVES);
      
      {
        // Erase slot and nav prompt from vram
        Mode3::Rect prompt_square(prompt_directions_message_width, prompt_directions_message_height, 
          SCREEN_WIDTH - prompt_directions_message_width, SCREEN_HEIGHT - prompt_directions_message_height);
        prompt_square.FillDraw(0);
        selection_square.FillDraw(0);
      }
      
      vsync();
      Display_Preview(saves + curr, curr+1);
      
      // Redraw slot sel UI.
      Mode3_printf(0, 0, (num_saves == MAX_SAVES) ? 0x001F : (num_saves > MAX_SAVES>>1) ? 0x02DF : 0x03E0, "[ %d / %d ] \x1b[0x7FFF]Slots Used", num_saves, MAX_SAVES);
      Draw_Save_Prompt_Directions();
      skip_nav_button_check = true;
    }

    if (!skip_nav_button_check) {
      if (KEY_PRESSED(L)) {
        DEBOUNCE_KEY(L);
        if (--curr < 0)
          curr = num_saves - 1;

      } else if (KEY_PRESSED(R)) {
        DEBOUNCE_KEY(R);
        if (++curr >= num_saves)
          curr = 0;
      } else {
        continue;
      }
    } else {
      skip_nav_button_check = false;
    }

    selection_square.FillDraw(SLOT_SEL_BG);
    selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_SEL_BORDER);
    Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
        selection_square.y + SLOT_BORDER_LEN, SLOT_SEL_FG, "%d) %s", curr+1, saves[curr].slot_name);



  }


}

void load_buf(bool_t *dst, const Save_Slot_t *src) {

  bool_t *dst_row;
  const u8_t *grid_state = src->grid_state, *src_row;
  u8_t curr_byte;
  for (int i = 0; i < GRID_HEIGHT; ++i) {
    dst_row = dst + i*GRID_WIDTH;
    src_row = grid_state + i*GRID_BITPACK_ROWLEN;
    
    for (int j = 0; j < GRID_BITPACK_ROWLEN; ++j) {  
      curr_byte = *src_row++;
      for (int k = 0; k < 8; ++k)
        *dst_row++ = (0!=(curr_byte&(1<<k)));
    }
  }
}

void save_buf(Save_Slot_t *dst, const bool_t *src) {
  const bool_t *src_row;
  u8_t *save_grid = dst->grid_state, *dst_row, curr_byte;
  for (int i = 0; i < GRID_HEIGHT; ++i) {
    src_row = src + GRID_WIDTH*i;
    dst_row = save_grid + GRID_BITPACK_ROWLEN*i;

    for (int j = 0; j < GRID_BITPACK_ROWLEN; ++j) {
      curr_byte = 0;
      for (int k = 0; k < 8; ++k)
        if (*src_row++)
          curr_byte |= (1<<k);

      *dst_row++ = curr_byte;
    }
  }

}


int Prompt_Save_Slot(Save_Slot_t *saves, int num_saves) {
  Mode3::Rect selection_square(SLOT_DISPLAY_WIDTH, SLOT_DISPLAY_HEIGHT, SLOT_X_ALIGNMENT, SLOT_Y_ALIGNMENT);
  if (saves == nullptr)
    selection_square.y = (SCREEN_HEIGHT - SLOT_DISPLAY_HEIGHT)>>1;
  selection_square.FillDraw(SLOT_SEL_BG);
  selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_SEL_BORDER);
  Draw_Save_Prompt_Directions();
  Mode3_puts(NEW_SAVE_TXT, selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD, selection_square.y + SLOT_BORDER_LEN, SLOT_SEL_FG);

  Mode3_printf(0, 0, (num_saves == MAX_SAVES) ? 0x001F : (num_saves > MAX_SAVES>>1) ? 0x02DF : 0x03E0, "[ %d / %d ] \x1b[0x7FFF]Slots Used", num_saves, MAX_SAVES);
  
  vsync();

  if (saves == nullptr) {
    while (1) {
      if (KEY_PRESSED(A)) {
        DEBOUNCE_KEY(A);
        return 0;
      } else if (KEY_PRESSED(B)) {
        DEBOUNCE_KEY(B);
        return -1;
      } else {
        continue;
      }
    }
  }

  int curr = 0, ret = 0;
  bool_t newslot = true, redraw_slot_dpy = false, redraw_after_preview = false, redraw_after_cursor_mv = false;
  // and vice versa:
  selection_square.y += SLOT_DISPLAY_HEIGHT;
  selection_square.FillDraw(SLOT_BG);
  selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_BORDER);

  Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
      selection_square.y + SLOT_BORDER_LEN, SLOT_FG, "%d) %s", curr+1, saves[curr].slot_name); 

  while (1) {
    vsync();

    if (KEY_PRESSED(A)) {
      DEBOUNCE_KEY(A);
      if (newslot) {
        if (num_saves < MAX_SAVES) {
          return num_saves;
        } else {
          Mode3_printf(0, 0, 0x001F, "Max save count reached (%d)", MAX_SAVES);
          int frame = 128;
          while (frame-- > 0)
            vsync();
          Mode3_printf(0, 0, 0, "Max save count reached (%d)", MAX_SAVES);
        }
      } else {
        return curr;
      }
    } else if (KEY_PRESSED(B)) {
      DEBOUNCE_KEY(B);
      return -1;
    }

    if (KEY_PRESSED(SEL)) {
      DEBOUNCE_KEY(SEL);
      if (newslot)
        continue;
      // Erase slot count 
      Mode3_printf(0, 0, 0, "[ %d / %d ] Slots Used", num_saves, MAX_SAVES);
      {
        // Erase slots display and nav prompt from vram 
        Mode3::Rect prompt_square(prompt_directions_message_width, prompt_directions_message_height, 
          SCREEN_WIDTH - prompt_directions_message_width, SCREEN_HEIGHT - prompt_directions_message_height);
        prompt_square.FillDraw(0);
        Mode3::Rect slots_square(SLOT_DISPLAY_WIDTH, SLOT_DISPLAY_HEIGHT*2, SLOT_X_ALIGNMENT, SLOT_Y_ALIGNMENT);
        slots_square.FillDraw(0);
      }
      vsync();
      Display_Preview(saves + curr, curr+1);
      
      // Redraw slot sel UI.
      Mode3_printf(0, 0, (num_saves == MAX_SAVES) ? 0x001F : (num_saves > MAX_SAVES>>1) ? 0x02DF : 0x03E0, "[ %d / %d ] \x1b[0x7FFF]Slots Used", num_saves, MAX_SAVES);
      Draw_Save_Prompt_Directions();
      redraw_after_preview = true;

    }
    if (!redraw_after_preview) {
      if (KEY_PRESSED(DOWN)) {
        DEBOUNCE_KEY(DOWN);
        newslot = !newslot;
        redraw_after_cursor_mv = true;
      } else if (KEY_PRESSED(UP)) {
        DEBOUNCE_KEY(UP);
        newslot = !newslot;
        redraw_after_cursor_mv = true;
      }
    }
    if (redraw_after_preview || redraw_after_cursor_mv) {
      if (newslot) {
        selection_square.y = SLOT_Y_ALIGNMENT;
        selection_square.FillDraw(SLOT_SEL_BG);
        selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_SEL_BORDER);
        Mode3_puts(NEW_SAVE_TXT, selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
            selection_square.y + SLOT_BORDER_LEN, SLOT_SEL_FG);
        
        // and vice versa:
        selection_square.y += SLOT_DISPLAY_HEIGHT;
        selection_square.FillDraw(SLOT_BG);
        selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_BORDER);
        Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
            selection_square.y + SLOT_BORDER_LEN, SLOT_FG, "%d) %s", curr+1, saves[curr].slot_name); 
      } else {
        // Overwrite previously-selected element with non-selected colors
        selection_square.y = SLOT_Y_ALIGNMENT;
        selection_square.FillDraw(SLOT_BG);
        selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_BORDER);
        Mode3_puts(NEW_SAVE_TXT, selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
            selection_square.y + SLOT_BORDER_LEN, SLOT_FG);        
        // and vice versa:
        selection_square.y += SLOT_DISPLAY_HEIGHT;
        selection_square.FillDraw(SLOT_SEL_BG);
        selection_square.OutlineDraw(SLOT_BORDER_LEN, SLOT_SEL_BORDER);
        Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
            selection_square.y + SLOT_BORDER_LEN, SLOT_SEL_FG, "%d) %s", curr+1, saves[curr].slot_name); 

      }
      redraw_after_preview = redraw_after_cursor_mv = false;
      continue;
    }

    if (KEY_PRESSED(L)) {
      DEBOUNCE_KEY(L);

      if (--curr < 0)
        curr = num_saves - 1;
      redraw_slot_dpy = num_saves != 1;
    } else if (KEY_PRESSED(R)) {
      DEBOUNCE_KEY(R);
      if (++curr >= num_saves)
        curr = 0;
      redraw_slot_dpy = num_saves != 1;
    }

    if (redraw_slot_dpy) {
      selection_square.y = SLOT_Y_ALIGNMENT + SLOT_DISPLAY_HEIGHT;
      selection_square.FillDraw(newslot ? SLOT_BG : SLOT_SEL_BG);
      selection_square.OutlineDraw(SLOT_BORDER_LEN, newslot ? SLOT_BORDER : SLOT_SEL_BORDER);

      Mode3_printf(selection_square.x + SLOT_BORDER_LEN + SLOT_TEXT_LEFT_PAD,
          selection_square.y + SLOT_BORDER_LEN, newslot?SLOT_FG:SLOT_SEL_FG, "%d) %s", curr+1, saves[curr].slot_name);
      redraw_slot_dpy = false;
    }
  } 
  return -1; 
}

bool_t State_SaveGame(bool_t *cur_buf) {
  int num_saves = 0;
  SRAM_Read(&num_saves, 0, 4);
  if (num_saves == -1 || num_saves > MAX_SAVES) {

    num_saves = 0;
    SRAM_Write(&num_saves, 0, 4);
  }
//  Mode3_printf(0,0,0x7fff, "%d", num_saves);
//  while (1);
  size_t saves_len = num_saves*sizeof(Save_Slot_t);
  Save_Slot_t *saves = nullptr;
  if (saves_len > 0) {
    saves = _g_saves;
    SRAM_Read(saves, 4, saves_len);
  }
  vsync();
  int outcome = Prompt_Save_Slot(saves, num_saves);
  if (outcome == -1) {
    return false;
  }
  
  CLEAR_SCREEN;
  vsync();
  if (outcome == num_saves) {
    // First, prompt user to give save state name.
    Save_Slot_t tmp = {0};
    
    if (0 > Mode3_gets(tmp.slot_name, 15)) {
      vsync();
      CLEAR_SCREEN;
      return false;
    }
    // Then increment save count.
    ++num_saves;
    SRAM_Write(&num_saves, 0, 4);
    vsync();
    CLEAR_SCREEN;
    save_buf(&tmp, cur_buf);
    SRAM_Write(&tmp, 4+saves_len, sizeof(tmp));
    return true;
  }

  Save_Slot_t tmp = saves[outcome];
  if (0 > Mode3_gets(tmp.slot_name, 15)) {
    vsync();
    CLEAR_SCREEN;
    return false;
  }
  vsync();
  CLEAR_SCREEN;
  save_buf(&tmp, cur_buf);
  SRAM_Write(&tmp, 4 + outcome*sizeof(Save_Slot_t), sizeof(Save_Slot_t));
  return true;
  
}




bool_t Load_SaveGame(bool_t *cur_buf) {
  Save_Slot_t *saves = nullptr;
  int num_saves = 0, outcome = -1;
  SRAM_Read(&num_saves, 0, 4);
  if (num_saves <= 0 || num_saves > MAX_SAVES) {
    num_saves = 0;
    SRAM_Write(&num_saves, 0, 4);
    Mode3_puts("No saves found on cartridge.", 0, 0, 0x001F);
    int wait = 128;
    while (--wait > 0)
      vsync();
    Mode3_puts("No saves found on cartridge.", 0, 0, 0x0000);
    return false;
  }
  size_t saves_len = sizeof(Save_Slot_t)*num_saves;
  saves = _g_saves;
  SRAM_Read(saves, 4, saves_len);
  outcome = Prompt_Load_Slot(saves, num_saves);
  if (0 > outcome) {
    return false;
  }

  load_buf(cur_buf, &saves[outcome]);
  CLEAR_SCREEN;
  return true;
}

