#ifndef _BMP_TYPES_HPP_
#define _BMP_TYPES_HPP_
#include "gba_types.h"


namespace Mode3 {

struct BMP {
  u16_t* pbuf;
  u16_t width, height;
  
  BMP(void);
  explicit BMP(u16_t* pbuf, u16_t width, u16_t height);
  BMP(const BMP&);

  void Draw(u16_t x, u16_t y);
};

struct Rect {
  Rect(void);
  explicit Rect(s16_t width, s16_t height=0, s16_t x=0, s16_t y=0);
  Rect(const Rect&);

  void FillDraw(u16_t color);

  void OutlineDraw(int size, u16_t color);

  void OutlineDrawLineByLine(int size, u16_t color);


  void SetDimms(s16_t width, s16_t height);

  
  void SetCoords(s16_t x, s16_t y);

  void Init(s16_t width, s16_t height, s16_t x, s16_t y);


  void FillDrawLineByLine(u16_t color);

  s16_t x, y, width, height;
};  /* struct Rect */


}  /* namespace Mode3 */



#endif  /* _BMP_TYPES_HPP_ */
