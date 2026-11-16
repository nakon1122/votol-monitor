// What the last crash was, read from the crash dump the system keeps in flash:
// the reason text ("assert failed: ..."), the task and the code address.
// Shown on the boot screen so a photo is enough to find the fault.
#include <Arduino.h>
#include "crashinfo.h"
#include <esp_idf_version.h>
#if __has_include(<esp_core_dump.h>)
#include <esp_core_dump.h>
#endif

bool crashInfo(char *reason, size_t rn, char *where, size_t wn) {
  reason[0] = where[0] = 0;
#if defined(CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH) && defined(CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF)
  if (esp_core_dump_image_check() != ESP_OK) return false;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
  if (esp_core_dump_get_panic_reason(reason, rn) != ESP_OK) reason[0] = 0;
#endif
  esp_core_dump_summary_t *s = (esp_core_dump_summary_t *)calloc(1, sizeof(esp_core_dump_summary_t));
  if (s && esp_core_dump_get_summary(s) == ESP_OK) {
#if defined(__riscv)
    snprintf(where, wn, "task %s pc %08lx ra %08lx cause %lu addr %08lx", s->exc_task, (unsigned long)s->exc_pc,
             (unsigned long)s->ex_info.ra, (unsigned long)s->ex_info.mcause, (unsigned long)s->ex_info.mtval);
#else
    int k = snprintf(where, wn, "task %s pc %08lx bt", s->exc_task, (unsigned long)s->exc_pc);
    for (uint32_t i = 0; i < s->exc_bt_info.depth && i < 5 && k < (int)wn - 10; i++)
      k += snprintf(where + k, wn - k, " %08lx", (unsigned long)s->exc_bt_info.bt[i]);
#endif
  }
  free(s);
  esp_core_dump_image_erase();  // shown once
  return reason[0] || where[0];
#else
  return false;
#endif
}
