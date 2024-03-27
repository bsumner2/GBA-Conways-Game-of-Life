#include "bmp_types.hpp"
#include "gba_bitfield_macros.h"
#include "gba_types.h"
#include "gba_def.h"
#include "gba_inlines.h"

namespace Mode3 {

  
BMP::BMP(void) : pbuf(nullptr), width(0), height(0) { return; }
BMP::BMP(u16_t* pbuf, u16_t width, u16_t height) : pbuf(pbuf), width(width), 
    height(height) { return; }
BMP::BMP(const BMP& bmp) : BMP(bmp.pbuf, bmp.width, bmp.height)  { return; }

void BMP::Draw(u16_t x, u16_t y) {
#if 1
  u16_t* row;
  u16_t* bmprow;
  if (x + width > SCREEN_WIDTH)
    return;
  if (y+height > SCREEN_HEIGHT)
    return;
  for (int i = 0; i < height; ++i) {
    row = &VIDEO_BUF[(y+i)*SCREEN_WIDTH + x];
    bmprow = &pbuf[i*width];
    for (int j = 0; j < width; ++j) {
      if (bmprow[j]&ALPHA_MASK)
        continue;
      row[j] = bmprow[j];
    }
  }
#else
  u16_t cur;
  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
      if ((cur=pbuf[i*width + j])&0x8000)
        continue;
      else
        VIDEO_BUF[(y+i)*SCREEN_WIDTH + x + j] = pbuf[i*width + j];
    
#endif
}

Rect::Rect(void) : x(0), y(0), width(0), height(0) {
  return;
}

Rect::Rect(const Rect& src) : Rect(src.width, src.height, src.x, src.y) {
  return;
}

Rect::Rect(s16_t width, s16_t height, s16_t x, s16_t y) : x(x), y(y), 
    width(width), height(height ? height : width) {
  return;
}

void Rect::FillDraw(u16_t color) {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width;++j)
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
  }
}

void Rect::OutlineDraw(int size, u16_t color) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < width; ++j) {
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
      VIDEO_BUF[j + x + (y+height - 1 - i)*SCREEN_WIDTH] = color;
    }
  }
  int lim = height - size;
  for (int i = size; i < lim; ++i) {
    for (int j = 0; j < size; ++j) {
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
      VIDEO_BUF[x + width - 1 - j + (i + y)*SCREEN_WIDTH] = color;
    }
  }
}

void Rect::OutlineDrawLineByLine(int size, u16_t color) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < width; ++j) {
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
      VIDEO_BUF[j + x + (y+height - 1 - i)*SCREEN_WIDTH] = color;
    }
    vsync();
  }
  int lim = height - size;
  for (int i = size; i < lim; ++i) {
    for (int j = 0; j < size; ++j) {
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
      VIDEO_BUF[x + width - 1 - j + (i + y)*SCREEN_WIDTH] = color;
      vsync();
    }
    vsync();
  }
}

void Rect::SetDimms(s16_t width, s16_t height) {
  this->width = width;
  this->height = height;
}

void Rect::SetCoords(s16_t x, s16_t y) {
  this->x = x;
  this->y = y;
}

void Rect::Init(s16_t width, s16_t height, s16_t x, s16_t y) {
  this->width = width;
  this->height = height;
  this->x = x;
  this->y = y;
}

void Rect::FillDrawLineByLine(u16_t color) {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width;++j)
      VIDEO_BUF[j + x + (i + y)*SCREEN_WIDTH] = color;
    vsync();
  }
}

}  /* namespace Mode3 */
