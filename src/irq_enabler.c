#include <gba_def.h>

extern IWRAM_CODE void isr(void);

void enable_irq(void) {
  REG_IME = 0;
  REG_IE = 1;
  REG_DISPLAY_STAT = 8;
  REG_ISR_MAIN = isr;
  REG_IME = 1;
}
