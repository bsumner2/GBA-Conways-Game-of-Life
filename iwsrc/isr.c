#include <gba_def.h>
#include <gba_util_macros.h>

IWRAM_CODE void isr(void) {
  u32_t ie_and_if = REG_IE;
  ie_and_if &= REG_IF;

  REG_IF = ie_and_if;
  REG_IFBIOS |= ie_and_if;
}
