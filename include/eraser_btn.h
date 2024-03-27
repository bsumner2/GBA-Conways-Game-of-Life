#ifndef _ERASER_BTN_H_
#define _ERASER_BTN_H_

#ifdef __cplusplus
extern "C" {
#endif  /* C++ Name Mangler guard */

// GBA Bitmap array. Uint16 Color word Formatted as 0bABBBBGGGGRRRR
// Bitmap, eraser_btn: dims = 16x16. arraylen = 256

#define eraser_btn_width 16
#define eraser_btn_height 16
extern const unsigned short eraser_btn[];

#ifdef __cplusplus
}
#endif  /* C++ Name Mangler guard */


#endif  /* _ERASER_BTN_H_ */
