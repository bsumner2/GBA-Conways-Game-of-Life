	.file	"tonc_ISR_Master_ctl.s"

#include "tonc_asminc.h"

@@ Credit: LibTonc's Fast memcpy 32 byte implementation
BEGIN_FUNC_ARM(fast_memcpy32, CSEC_IWRAM)
  @@ Regs
  @@ r0 Dest ptr
  @@ r1 Src ptr
  @@ r2 word count
  AND r12, r2, #7  @@ r12 will have remainder words
  MOVS r2, r2, LSR #3  @@ r2 will have word block count
  BEQ .Lremainder_fcpy32
  PUSH {r4-r10}
.Lmainloop_fcpy32:
    LDMIA r1!, { r3-r10 }
    STMIA r0!, { r3-r10 }
    SUBS r2, #1
    BNE .Lmainloop_fcpy32
  POP {r4-r10}
.Lremainder_fcpy32:
    SUBS r12, #1
    LDRCS r3, [r1], #4
    STRCS r3, [r0], #4
    BCS .Lremainder_fcpy32
  BX lr
END_FUNC(fast_memcpy32)

BEGIN_FUNC_ARM(fast_memset32, CSEC_IWRAM)
  @@ Regs
  @@ r0 Dest
  @@ r1 Fill Value
  @@ r2 Word count

  AND r12, r2, #7  @@ r12 = 0b111&word_ct = word_ct%8 = remainder words that can't be block filled.
  MOVS r2, r2, LSR #3  @@ r2 = r2>>3 = word_ct>>3 = word_ct/8 = word_block_ct
  BEQ .Lremainder_fmset32

  PUSH {r4-r10}
  MOV r3, r1
  MOV r4, r1
  MOV r5, r1
  MOV r6, r1
  MOV r7, r1
  MOV r8, r1
  MOV r9, r1
  MOV r10, r1
.Lmainloop_fmset32:
    STMIA r0!, {r3-r10}
    SUBS r2, #1
    BNE .Lmainloop_fmset32
  POP {r4-r10}
.Lremainder_fmset32:
    SUBS r12, #1
    STRCS r1, [r0], #4
    BCS .Lremainder_fmset32
  BX lr
END_FUNC(fast_memset32)


BEGIN_FUNC_THUMB(BIOS_vsync, CSEC_TEXT)
  SWI #0x05
  BX lr
END_FUNC(BIOS_vsync)
