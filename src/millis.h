#pragma once

#include <stdint.h>

static inline uint32_t millis() {
    return to_ms_since_boot(get_absolute_time());
}
