#ifndef _BURTANA_H_
#define _BURTANA_H_

#ifdef __cplusplus
extern "C" {
#endif  /* CXX Name Mangler Guard */

#define burtana_CellWidth 8
#define burtana_CellHeight 16
#define burtana_GlyphHeight 12
#define burtana_GlyphCount 96

/* Uniform cell size (in bytes) for each glyph.
 * (Use as offset for glyph indexing): */
#define burtana_CellSize 16

/* Glyph pixel data. 1bpp. */
extern const unsigned short burtana_GlyphData[768];
extern const unsigned char burtana_GlyphWidths[96];
#ifdef __cplusplus
}
#endif  /* CXX Name Mangler Guard */

#endif  /* _BURTANA_H_ */

