#ifndef _GBA_INLINES_H_
#define _GBA_INLINES_H_

#ifdef __cplusplus
extern "C" {
#endif  /* Name mangler protection */


#include "gba_def.h"
#include "gba_types.h"
#include "gba_bitfield_macros.h"
#include "gba_util_macros.h"

extern void BIOS_vsync(void);

STAT_INLN void vsync(void) {
#ifdef USE_BIOS_VSYNC
  BIOS_vsync();
#else
  while (REG_DISPLAY_VCOUNT >= 160);  // Get out of VBlank if not already.
  while (REG_DISPLAY_VCOUNT < 160);  // Wait till next VBlank before continuing.
#endif
}



STAT_INLN void OAM_SetDims(Obj_Attr_t* attr, ObjectDims dims) {
  attr->attr0.shape = 3&(dims>>2);
  attr->attr1.regular.size = 3&dims;
}


STAT_INLN void ObjAttr_AffineInit(Obj_Attr_t* obj, u16_t affine_idx) {
  obj->attr0.object_mode = OA0_AFFINE;
  obj->attr1.affine.affine_idx = affine_idx;
}

STAT_INLN void ObjAttr_SetPosition(Obj_Attr_t* obj, u16_t x, u16_t y) {
  obj->attr0.y_coord = y;
  obj->attr1.shared_attributes.x_coord = x;
}

STAT_INLN void ObjAttr_Hide(Obj_Attr_t* obj) {
  obj->attr0.object_mode = OA0_HIDDEN;
}

STAT_INLN void ObjAttr_UnHide(Obj_Attr_t* obj, u16_t object_mode) {
  obj->attr0.object_mode =  object_mode & 1;
}

STAT_INLN void ObjAttr_SetPaletteBankIdx(Obj_Attr_t* obj, u16_t palbank_idx) {
  obj->attr2.pal_idx = palbank_idx; 
}

#define DEBOUNCE_KEY(key) while (KEY_PRESSED(key))


#ifdef __cplusplus
}
#endif  /* Name mangler protection */

#endif  /* _GBA_INLINES_H_ */
