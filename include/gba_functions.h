#ifndef _GBA_FUNCTIONS_H_
#define _GBA_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* C++ Name Mangler guard */

#include "gba_def.h"
#include "gba_types.h"
#include "gba_bitfield_macros.h"
#include <stdarg.h>

// Opted for minimalist approach for SRAM interface. Essentially just acts as
// an interface for doing read/write operations to in-cartridge SRAM 
// byte-by-byte, since the bus width for the SRAM is only 8b. Feel
// free to use the macro SRAM_BUF to do R/W manually, just keep in mind the
// bus width constraint.
void _Internal_DisplayControl_SetModes(u32_t mode1, ...);
#define REG_DISPLAY_CNT_SET_MODES(...) \
  _Internal_DisplayControl_SetModes(__VA_ARGS__, 0xFFFFFFFF)

void SRAM_Write(const void* data, u16_t offset, u16_t len);


void SRAM_Read(void* dest, u16_t offset, u16_t len);


void OAM_Init(Obj_Attr_t* obj_buf, u32_t obj_ct);

void OAM_Copy(Obj_Attr_t* dest, const Obj_Attr_t* src, u32_t ct);

Obj_Attr_t* ObjAttr_Init(Obj_Attr_t* obj, u16_t object_mode, 
    u16_t graphics_mode, ObjectDims dims, u16_t x, u16_t y, bool_t mosaic,
    bool_t depth_8bpp, u16_t palette_idx, u16_t priority, u16_t tile_idx, 
    u16_t affine_idx);

Obj_Attr_t* ObjAttr_QuickInit(Obj_Attr_t* obj, ObjectDims dims, u16_t pal_idx, 
    u16_t tile_idx, bool_t depth_8bpp);

void BgCnt_Init(Bg_Controller_t* cnt, u16_t priority,
    u16_t tiles_offset, bool_t is_mosaic, bool_t is_8bpp, u16_t scr_ent_offset,
    bool_t affine_wrap, u16_t background_size);

void BgOffset_Set(u16_t bgno, Coordinate coord);

void* DMA_Memcpy(void* dest, const void* src, u32_t dma_slot, 
    DMA_Controller_t ctl);


void* DMA_Memcpy2(void* dest, const void* src, u32_t dma_slot, 
    u16_t chunk_ct, u16_t dest_adj_mode, u16_t src_adj_mode, bool_t repeat,
    bool_t word_len_chunks, u16_t timing_mode, bool_t interrupt_after);



#ifdef __cplusplus
}
#endif  /* C++ Name Mangler guard */

#endif  /* _GBA_FUNCTIONS_H_ */
