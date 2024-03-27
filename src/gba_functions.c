#include "gba_types.h"
#include "gba_def.h"
#include "gba_bitfield_macros.h"
#include "gba_functions.h"
#include <stddef.h>

void SRAM_Read(void* dest, u16_t offset, u16_t len) {
  u8_t* dst, * src, * end;
  if (!len)
    return;
  
  src = (SRAM_BUF + offset), dst = (u8_t*)dest, end = src + len;

  do *(dst++) = *(src++); while (src != end);
}

void SRAM_Write(const void* data, u16_t offset, u16_t len) {
  u8_t* src, * dest, * end;
  if (!len)
    return;
  
  src = (u8_t*)data, dest = (SRAM_BUF + offset), end = dest + len; 
  
  do *(dest++) = *(src++); while (dest != end);
}


void OAM_Init(Obj_Attr_t* obj_buf, u32_t obj_ct) {
  u32_t* obj_raw = (u32_t*)obj_buf, ct = obj_ct+1;

  // Typecasting to raw int type array allows for us to 
  // zero out the whole field without issue.
  while (--ct) {
    *(obj_raw++) = OA0_HIDDEN<<OA0_OBJ_MODE_SHIFT;
    *(obj_raw++) = 0;
  }
  OAM_Copy(OAM_BUF, obj_buf, obj_ct);
}

void OAM_Copy(Obj_Attr_t* dest, const Obj_Attr_t* src, u32_t ct) {
#ifndef __OAM_USE_RAW_WORD_COPY
  while (ct--)
    *(dest++) = *(src++);
#else
  u32_t* dest_raw = (u32_t*)dest, * src_raw = (u32_t*)src;
  while (ct--) {
    *(dest_raw++) = *(src_raw++);
    *(dest_raw++) = *(src_raw++);
  }
#endif
}

STAT_INLN void ObjAttr0_Set(ObjAttr0_t* attr, u16_t y_coord, u16_t object_mode,
    u16_t graphics_mode, bool_t mosaic, bool_t depth_8bpp, u16_t shape) {
  attr->y_coord  = y_coord; attr->object_mode = object_mode; 
  attr->graphics_mode = graphics_mode; attr->mosaic_flag = mosaic;
  attr->color_depth8 = depth_8bpp; attr->shape = shape;
}

STAT_INLN void ObjAttr1_Set(SharedAttr1_t* attr, u16_t x_coord, u16_t size) {
  attr->x_coord = x_coord;
  attr->size = size;
}

STAT_INLN void ObjAttr2_Set(ObjAttr2_t* attr, u16_t tile_idx, u16_t priority, 
    u16_t pal_idx) {
  attr->tile_idx = tile_idx; attr->pal_idx = pal_idx; attr->priority = priority;
}

Obj_Attr_t* ObjAttr_Init(Obj_Attr_t* obj, u16_t object_mode, 
    u16_t graphics_mode, ObjectDims dims, u16_t x, u16_t y, bool_t mosaic,
    bool_t depth_8bpp, u16_t palette_idx, u16_t priority, u16_t tile_idx, 
    u16_t affine_idx) {

  ObjAttr0_Set(&(obj->attr0), y, object_mode > 2 ? OA0_HIDDEN : object_mode, 
      graphics_mode > 2 ? OA0_GFX_NORMAL : graphics_mode, mosaic, depth_8bpp,
      dims>>2);
  

  ObjAttr1_Set(&(obj->attr1.shared_attributes), x, dims);
  ObjAttr2_Set(&(obj->attr2), tile_idx, priority, palette_idx);

  if (object_mode == OA0_AFFINE)
    obj->attr1.affine.affine_idx = affine_idx;

  return obj;
}

Obj_Attr_t* ObjAttr_QuickInit(Obj_Attr_t* obj, ObjectDims dims, u16_t pal_idx, 
    u16_t tile_idx, bool_t depth_8bpp) {
  ObjAttr0_Set(&(obj->attr0), 0, OA0_REGULAR, OA0_GFX_NORMAL, 0, depth_8bpp,
      dims>>2);
  ObjAttr1_Set(&(obj->attr1.shared_attributes), 0, dims);
  ObjAttr2_Set(&(obj->attr2), tile_idx, 0, pal_idx);


  return obj;
}

void _Internal_DisplayControl_SetModes(u32_t mode1, ...) {
  va_list args;
  u32_t next_arg;
  u32_t val;
  va_start(args, mode1);
  REG_DISPLAY_CNT = mode1;
  while ((val = ~(next_arg = va_arg(args, u32_t))))
    REG_DISPLAY_CNT |= next_arg;
  va_end(args);
}


void BgCnt_Init(Bg_Controller_t* cnt, u16_t priority,
    u16_t tiles_offset, bool_t is_mosaic, bool_t is_8bpp, u16_t scr_ent_offset,
    bool_t affine_wrap, u16_t background_size) {
  *cnt = (Bg_Controller_t) {
    .priority = priority,
    .tblock_offset = tiles_offset,
    .mosaic_flag = is_mosaic,
    .color_depth8 = is_8bpp,
    .scrblock_offset = scr_ent_offset,
    .affine_wrap_flag = affine_wrap,
    .size = background_size
  };  
}

void BgOffset_Set(u16_t bgno, Coordinate coord) {
  REG_BG_OFS[bgno] = coord;
}

void* DMA_Memcpy(void* dest, const void* src, u32_t dma_slot, 
    DMA_Controller_t ctl) {
  if (dma_slot&0xFFFFFFFC)
    return NULL;
  ctl.enable_flag = true;
  REG_DMA[dma_slot] = 
    (DMA_Handle_t){.src = src, 
      .dest = dest, .controller = ctl};
  return dest;
}
void* DMA_Memcpy2(void* dest, const void* src, u32_t dma_slot, 
    u16_t chunk_ct, u16_t dest_adj_mode, u16_t src_adj_mode, bool_t repeat,
    bool_t word_size_chunks, u16_t timing_mode, bool_t interrupt_after) {
  if (dma_slot&0xFFFFFFC)
    return NULL;
  REG_DMA[dma_slot] = ((DMA_Handle_t) {
    src,
    dest,
    ((DMA_Controller_t) {
      .transfer_ct = chunk_ct,
      .dest_adj = dest_adj_mode,
      .src_adj = src_adj_mode,
      .repeat_flag = repeat,
      .chunk_size32 = word_size_chunks,
      .timing_mode = timing_mode,
      .interrupt_flag = interrupt_after,
      .enable_flag = true,
    })
  });
  return dest;
}

