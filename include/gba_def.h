#ifndef _GBA_DEF_H_
#define _GBA_DEF_H_


#include "gba_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* extern "C" name mangler guard */

#define SCREEN_WIDTH      240
#define SCREEN_HEIGHT     160

#define MEM_IO            0x04000000
#define MEM_PAL           0x05000000
#define MEM_VRAM          0x06000000
#define MEM_OAM           0x07000000
#define MEM_GAMEPAK_SRAM  0x0E000000

#define PAL_BG_MEMBLOCK_LEN   0x00200
#define PAL_OBJ_MEMBLOCK_LEN  0x00200

#define MEM_PAL_OBJ (MEM_PAL + PAL_BG_MEMBLOCK_LEN)

#define PAL_BG_BUF  ((u16_t*) MEM_PAL)
#define PAL_OBJ_BUF ((u16_t*) MEM_PAL_OBJ)


#define REG_DISPLAY_CNT *((vu32_t*) MEM_IO)
#define REG_DISPLAY_STAT *((vu16_t*) (MEM_IO + 0x0004))
#define REG_DISPLAY_VCOUNT *((vu16_t*) (MEM_IO + 0x0006))
#define SRAM_BUF ((u8_t*) (MEM_GAMEPAK_SRAM))


#define BG_AFFINE_PARAMS_MEM_OFFSET 0x0000
#define BG_CNT_MEM_OFFSET           0x0008
#define BG_HOFS_MEM_OFFSET          0x0010
#define BG_VOFS_MEM_OFFSET          0x0012

#define REG_BG_AFFINE_PARAMS \
  ((BgAffine_t*) (MEM_IO + BG_AFFINE_PARAMS_MEM_OFFSET))

#define REG_BG_CNT ((vu16_t*) (MEM_IO + BG_CNT_MEM_OFFSET))
#define REG_BG_CONTROLLER ((Bg_Controller_t*) (MEM_IO + BG_CNT_MEM_OFFSET))

#define REG_BG_OFS ((Coordinate*) (MEM_IO + BG_HOFS_MEM_OFFSET))
#define REG_BG_HOFS(bgno) *((vu16_t*) (MEM_IO + BG_HOFS_MEM_OFFSET + (bgno<<2)))
#define REG_BG_VOFS(bgno) *((vu16_t*) (MEM_IO + BG_VOFS_MEM_OFFSET + (bgno<<2)))


#define BG_SCR_BLOCK_BUF ((ScreenBlock*) MEM_VRAM)

#define VIDEO_BUF ((u16_t*) MEM_VRAM)
#define TILE_BUF ((TBlock*) MEM_VRAM)
#define SPR_TILE_BUF (TILE_BUF + 4)
#define SPR_TILE8_BUF ((TBlock8*) MEM_VRAM)

#define OAM_BUF       ((Obj_Attr_t*) MEM_OAM)
#define OAM_AFF_BUF   ((Obj_Affine_t*) MEM_OAM)


#define REG_KEYINPUT *((vu32_t*) (MEM_IO + 0x0130))

#define REG_DMA ((volatile DMA_Handle_t*) (MEM_IO + 0x00B0))

#define REG_TM ((volatile Timer_Handle_t*) (MEM_IO + 0x0100))

#define REG_IME *((vu16_t*) (MEM_IO + 0x0208))
#define REG_IE  *((vu16_t*) (MEM_IO + 0x0200))
#define REG_IF  *((vu16_t*) (MEM_IO + 0x0202))

#define REG_IE_FIELDS *((volatile Interrupt_Enabler_t*) (MEM_IO + 0x0200))
#define REG_IF_FIELDS *((volatile Interrupt_Enabler_t*) (MEM_IO + 0x0202))
#define MEM_ISR_CALLBACK_ADDR 0x03007FFC

#define REG_ISR_MAIN *((IRQ_Callback_t*) (MEM_ISR_CALLBACK_ADDR))


#ifdef __cplusplus
}
#endif  /* extern "C" name mangler guard */

#endif  /* _GBA_DEF_H_ */
