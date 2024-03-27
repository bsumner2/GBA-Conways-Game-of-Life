#ifndef _GBA_TYPES_H_
#define _GBA_TYPES_H_


#include "sys/_stdint.h"
#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif  /* extern "C" name mangler guard */

#include "gba_util_macros.h"


typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long int u64_t;

typedef signed char s8_t;
typedef signed short s16_t;
typedef signed int s32_t;
typedef signed long int s64_t;



typedef volatile u8_t vu8_t;
typedef volatile u16_t vu16_t;
typedef volatile u32_t vu32_t;
typedef u8_t bool_t;

typedef struct Coord Coordinate;
typedef struct _4BPP_Tile Tile, Tile4;
typedef struct _8BPP_Tile Tile8;


struct Coord {
  s16_t x, y;
} ALIGN(4);

struct _4BPP_Tile {
  u32_t buf[8];
};

struct _8BPP_Tile {
  u32_t buf[16];
};


typedef Tile TBlock[512];
typedef Tile8 TBlock8[256];

typedef struct OAM_Attr0_Bitfield ObjAttr0_t;
typedef struct OAM_Regular_Attr1_Bitfield RegAttr1_t;
typedef struct OAM_Affine_Attr1_Bitfield AffineAttr1_t;
typedef struct OAM_Attr2_Bitfield ObjAttr2_t;
typedef union OAM_Regular_Affine_Attr1_Union ObjAttr1_t;
typedef struct OAM_Affine_Matrix_MemMap  Obj_Affine_t;
typedef struct OAM_Attributes_MemMap Obj_Attr_t;
typedef struct OAM_RegAff_Shared_Attr1 SharedAttr1_t;

struct OAM_Attr0_Bitfield {
  u16_t y_coord : 8;
  u16_t object_mode : 2;
  u16_t graphics_mode : 2;
  u16_t mosaic_flag : 1;  /// Set high for mosaic effect.
  u16_t color_depth8 : 1;  /// Set high for 8bpp otherwise 4bpp
  u16_t shape : 2;
} ALIGN(2);

struct OAM_Regular_Attr1_Bitfield {
  u16_t x_coord : 9;
  u16_t pad0 : 3;
  u16_t horizontal_flip : 1;
  u16_t vertical_flip : 1;
  u16_t size : 2;
} ALIGN(2);

struct OAM_Affine_Attr1_Bitfield {
  u16_t x_coord : 9;
  u16_t affine_idx : 5;
  u16_t size : 2;
} ALIGN(2);

struct OAM_RegAff_Shared_Attr1 {
  u16_t x_coord : 9;
  u16_t pad0 : 5;
  u16_t size : 2;
};

union OAM_Regular_Affine_Attr1_Union {
  SharedAttr1_t shared_attributes;
  RegAttr1_t regular;
  AffineAttr1_t affine;
};

struct OAM_Attr2_Bitfield {
  u16_t tile_idx : 10;
  u16_t priority : 2;
  u16_t pal_idx : 4;  /// Ignored if in 8bpp mode set by ObjAttr0_t.color_depth8
} ALIGN(2);

struct OAM_Affine_Matrix_MemMap {
  u16_t pad0[3];
  s16_t pa;
  u16_t pad1[3];
  s16_t pb;
  u16_t pad2[3];
  s16_t pc;
  u16_t pad3[3];
  s16_t pd;
} ALIGN(4);

struct OAM_Attributes_MemMap {
  ObjAttr0_t attr0;
  ObjAttr1_t attr1;
  ObjAttr2_t attr2;
  s16_t pad1;
} ALIGN(4);

typedef struct BG_Affine_Params_t BgAffine_t;
typedef struct Bg_Controller Bg_Controller_t;
typedef u16_t ScreenBlock[1024];

struct Bg_Controller {
  u16_t priority : 2;
  u16_t tblock_offset : 2; 
  u16_t pad0 : 2;
  u16_t mosaic_flag : 1;
  u16_t color_depth8 : 1;
  u16_t scrblock_offset : 5;
  u16_t affine_wrap_flag : 1;
  u16_t size : 2;
} PACKED;

struct BG_Affine_Params {
    s16_t pa, pb;
    s16_t pc, pd;
    s32_t dx, dy;
} ALIGN(4);

typedef struct DMA_Ctl DMA_Controller_t;
typedef struct DMA_Hndl DMA_Handle_t;

struct DMA_Ctl {
  u16_t transfer_ct : 16;
  u16_t pad0 : 5;
  u16_t dest_adj : 2;
  u16_t src_adj : 2;
  u16_t repeat_flag : 1;
  u16_t chunk_size32 : 1;
  u16_t pad1 : 1;
  u16_t timing_mode : 2;
  u16_t interrupt_flag : 1;
  u16_t enable_flag : 1;
} ALIGN(4);


struct DMA_Hndl {
  const void* src;
  void* dest;
  DMA_Controller_t controller;
};

typedef struct Timer_Ctl Timer_Controller_t;
typedef struct Timer_Hndl Timer_Handle_t;

struct Timer_Ctl {
  u16_t frequency : 2;
  u16_t cascade_mode_flag : 1;
  u16_t pad0 : 3;
  u16_t interrupt_flag : 1;
  u16_t enable_flag : 1;
  u16_t pad1 : 8;
};

struct Timer_Hndl {
  Timer_Controller_t controller;
  u16_t data;
};


typedef void (*IRQ_Callback_t)(void);
typedef struct Interrupt_En Interrupt_Enabler_t;

struct Interrupt_En {
  u16_t vblank : 1;
  u16_t hblank : 1;
  u16_t vcount : 1;
  u16_t timer : 4;
  u16_t serial_communication : 1;
  u16_t direct_mem_access : 4;
  u16_t keypad : 1;
  u16_t cartridge : 1;
  u16_t pad0 : 2;
};

#ifdef __cplusplus
}
#endif  /* extern "C" name mangler guard */

#endif  /* _GBA_TYPES_H_ */
