#pragma once
#include <stddef.h>
// Reason and place of the last crash (from the flash crash dump); false when there is none.
bool crashInfo(char *reason, size_t rn, char *where, size_t wn);
