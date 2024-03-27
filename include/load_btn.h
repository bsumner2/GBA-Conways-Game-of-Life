#ifndef _LOAD_BTN_H_
#define _LOAD_BTN_H_

#ifdef __cplusplus
extern "C" {
#endif  /* C++ Name Mangler guard */

// GBA Bitmap array. Uint16 Color word Formatted as 0bABBBBGGGGRRRR
// Bitmap, load_btn: dims = 16x16. arraylen = 256

#define load_btn_width 16
#define load_btn_height 16
extern const unsigned short load_btn[];

#ifdef __cplusplus
}
#endif  /* C++ Name Mangler guard */


#endif  /* _LOAD_BTN_H_ */
