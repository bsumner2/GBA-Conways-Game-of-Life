#ifndef _CLR_BTN_H_
#define _CLR_BTN_H_

#ifdef __cplusplus
extern "C" {
#endif  /* C++ Name Mangler guard */

// GBA Bitmap array. Uint16 Color word Formatted as 0bABBBBGGGGRRRR
// Bitmap, clr_btn: dims = 16x16. arraylen = 256

#define clr_btn_width 16
#define clr_btn_height 16
extern const unsigned short clr_btn[];

#ifdef __cplusplus
}
#endif  /* C++ Name Mangler guard */


#endif  /* _CLR_BTN_H_ */
