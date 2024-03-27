#ifndef _GBA_BITFIELD_MACROS_H_
#define _GBA_BITFIELD_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* CXX Name Mangler guard */
// Display control register bitfields

#define DCNT_V_MODE0   0x0000
#define DCNT_V_MODE1   0x0001
#define DCNT_V_MODE2   0x0002
#define DCNT_V_MODE3   0x0003
#define DCNT_V_MODE4   0x0004
#define DCNT_V_MODE5   0x0005

#define DCNT_BG_MODE0  0x0100
#define DCNT_BG_MODE1  0x0200
#define DCNT_BG_MODE2  0x0400
#define DCNT_BG_MODE3  0x0800

#define DCNT_OBJ_MODE  0x1000

#define DCNT_1D_OBJ_ARRAY 0x0040
#define DCNT_2D_OBJ_ARRAY 0x0000

// Color hword channel masks
#define ALPHA_MASK 0x8000
#define BLUE_MASK  0x7C00
#define GREEN_MASK 0x03E0
#define RED_MASK   0x001F

// Object attribute memory bitfields

#define OA0_Y_COORD_MASK      0x00FF
#define OA0_OBJ_MODE_MASK     0x0300
#define OA0_GFX_MODE_MASK     0x0C00
#define OA0_MOSAIC_FLAG_MASK  0x1000
#define OA0_8BPP_FLAG_MASK    0x2000
#define OA0_SHAPE_MASK        0xC000

#define OA0_REGULAR       0x00
#define OA0_AFFINE        0x01
#define OA0_HIDDEN        0x02
#define OA0_AFFINE_DBL_RA 0x03

#define OA0_GFX_NORMAL  0x00
#define OA0_GFX_BLEND   0x01
#define OA0_GFX_WINDOW  0x02

#define OA0_Y_COORD_SHIFT      0
#define OA0_OBJ_MODE_SHIFT     8
#define OA0_GFX_MODE_SHIFT    10
#define OA0_MOSIAC_FLAG_SHIFT 12
#define OA0_8BPP_FLAG_SHIFT   13
#define OA0_SHAPE_SHIFT       14

#define OA1_X_COORD_MASK    0x01FF
#define OA1_AFFINE_IDX_MASK 0x3E00
#define OA1_HFLIP_MASK      0x1000
#define OA1_VFLIP_MASK      0x2000
#define OA1_SIZE_MASK       0xC000

typedef enum {
  OBJ_DIMS_8X8=0,
  OBJ_DIMS_16X16,
  OBJ_DIMS_32X32,
  OBJ_DIMS_64X64,
  OBJ_DIMS_16X8,
  OBJ_DIMS_32X8,
  OBJ_DIMS_32X16,
  OBJ_DIMS_64X32,
  OBJ_DIMS_8X16,
  OBJ_DIMS_8X32,
  OBJ_DIMS_16X32,
  OBJ_DIMS_32X64,
  OBJ_DIMS_NO_CHANGE=16,
} ObjectDims;

#define OAM_DIM_TO_SHAPE_FLAG(dim) ((dim>>2)&3)
#define OAM_DIM_TO_SIZE_FLAG(dim)  ((dim&3))


#define OA1_HFLIP_FLAG_SHIFT 12
#define OA1_VFLIP_FLAG_SHIFT 13

#define OA2_TILE_IDX_MASK     0x03FF
#define OA2_PRIORITY_MASK     0x0C00
#define OA2_PAL_BANK_IDX_MASK 0xF000

#define OA2_TILE_IDX_SHIFT      0
#define OA2_PRIORITY_SHIFT     10
#define OA2_PAL_BANK_SHIFT     12


// Background control register bitfield

#define BGCNT_PRIORITY_SHIFT             0
#define BGCNT_TBLOCK_OFFSET_SHIFT        2
#define BGCNT_MOSAIC_FLAG_SHIFT          6
#define BGCNT_COLOR_DEPTH8_SHIFT         7
#define BGCNT_SCREEN_ENTRY_OFFSET_SHIFT  8
#define BGCNT_AFFINE_WRAP_FLAG_SHIFT    13
#define BGCNT_SIZE_SHIFT                14


#define BGCNT_PRIORITY_MASK             0x0003
#define BGCNT_TBLOCK_OFFSET_MASK        0x000C
#define BGCNT_MOSAIC_FLAG_MASK          0x0040
#define BGCNT_COLOR_DEPTH8_MASK         0x0080
#define BGCNT_SCREEN_ENTRY_OFFSET_MASK  0x1F00
#define BGCNT_AFFINE_WRAP_FLAG_MASK     0x2000
#define BGCNT_SIZE_MASK                 0xC000


#define BGCNT_REG_32X32 0x0000
#define BGCNT_REG_64X32 0x0001
#define BGCNT_REG_32X64 0x0002
#define BGCNT_REG_64X64 0x0003

#define BGCNT_AFF_16X16   0x0000
#define BGCNT_AFF_32X32   0x0001
#define BGCNT_AFF_64X64   0x0002
#define BGCNT_AFF_128X128 0x0003

// Key input register masks
#define KEY_A         0x0001
#define KEY_B         0x0002
#define KEY_SEL       0x0004
#define KEY_START     0x0008
#define KEY_RIGHT     0x0010
#define KEY_LEFT      0x0020
#define KEY_UP        0x0040
#define KEY_DOWN      0x0080
#define KEY_R         0x0100
#define KEY_L         0x0200

#define KEY_PRESSED(key) (!(REG_KEYINPUT & KEY_##key))

// Direct Mem Access
#define DMA_ADJ_INC         0x0000
#define DMA_ADJ_DEC         0x0001
#define DMA_ADJ_FIXED       0x0002
#define DMA_ADJ_DEST_RELOAD 0x0003

#define DMA_IMMEDIATE   0x0000
#define DMA_ON_VBLANK   0x0001
#define DMA_ON_HBLANK   0x0002
#define DMA_ON_REFRESH  0x0003

#define DMA_TRANSFER_COUNT_MASK 0x0000FFFF
#define DMA_DEST_ADJ_MASK       0x00600000
#define DMA_SRC_ADJ_MASK        0x01800000
#define DMA_REPEAT_FLAG_MASK    0x02000000
#define DMA_CHUNK_SIZE32_MASK   0x04000000
#define DMA_TIMING_MODE_MASK    0x30000000
#define DMA_INTERRUPT_FLAG_MASK 0x40000000
#define DMA_ENABLE_FLAG_MASK    0x80000000

#define DMA_TRANSFER_COUNT_SHIFT  0
#define DMA_DEST_ADJ_SHIFT       21
#define DMA_SRC_ADJ_SHIFT        23
#define DMA_REPEAT_FLAG_SHIFT    25
#define DMA_CHUNK_SIZE32_SHIFT   26
#define DMA_TIMING_MODE_SHIFT    28
#define DMA_INTERRUPT_FLAG_SHIFT 30
#define DMA_ENABLE_FLAG_SHIFT    31

// Timers 

#define TIMER_FREQ_1    0
#define TIMER_FREQ_64   1
#define TIMER_FREQ_256  2
#define TIMER_FREQ_1024 3

#define TIMER_FREQ_MASK               0x0003
#define TIMER_CASCADE_MODE_FLAG_MASK  0x0004
#define TIMER_INTERRUPT_FLAG_MASK     0x0040
#define TIMER_ENABLE_FLAG_MASK        0x0080

#define TIMER_FREQ_SHIFT               0
#define TIMER_CASCADE_MODE_FLAG_SHIFT  2
#define TIMER_INTERRUPT_FLAG_SHIFT     6
#define TIMER_ENABLE_FLAG_SHIFT        7

#define INTERRUPT_VBLANK_FLAG_SHIFT     0
#define INTERRUPT_HBLANK_FLAG_SHIFT     1
#define INTERRUPT_VCOUNT_FLAG_SHIFT     2
#define INTERRUPT_TIMER_FLAG_SHIFT      3
#define INTERRUPT_SCOMM_FLAG_SHIFT      7
#define INTERRUPT_DMA_FLAG_SHIFT        8
#define INTERRUPT_KEYPAD_FLAG_SHIFT    12
#define INTERRUPT_CARTRIDGE_FLAG_SHIFT 13

#define INTERRUPT_VBLANK_FLAG_MASK    0x0001
#define INTERRUPT_HBLANK_FLAG_MASK    0x0002
#define INTERRUPT_VCOUNT_FLAG_MASK    0x0004
#define INTERRUPT_TIMER_FLAG_MASK     0x0078
#define INTERRUPT_SCOMM_FLAG_MASK     0x0080
#define INTERRUPT_DMA_FLAG_MASK       0x0F00
#define INTERRUPT_KEYPAD_FLAG_MASK    0x1000
#define INTERRUPT_CARTRIDGE_FLAG_MASK 0x2000


#ifdef __cplusplus
}
#endif  /* CXX Name Mangler guard */
#endif  /* _GBA_BITFIELD_MACROS_H_ */
