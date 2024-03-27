#ifndef _SAVE_BTN_H_
#define _SAVE_BTN_H_

#ifdef __cplusplus
extern "C" {
#endif  /* C++ Name Mangler guard */

// GBA Bitmap array. Uint16 Color word Formatted as 0bABBBBGGGGRRRR
// Bitmap, save_btn: dims = 16x16. arraylen = 256

#define save_btn_width 16
#define save_btn_height 16
extern const unsigned short save_btn[];

#ifdef __cplusplus
}
#endif  /* C++ Name Mangler guard */


#endif  /* _SAVE_BTN_H_ */
