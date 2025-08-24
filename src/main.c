#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "tusb.h"

#include "led.h"

void swap_case(uint32_t num_bytes, uint8_t* buf) {
    uint32_t i;

    for (i = 0; i < num_bytes; i++) {
        if (isalpha(buf[i])) {
            buf[i] ^= 0x20;
        }
    }
}

void cdc_task(void) {
    uint8_t buf[64];

    // stdio is running on interface 0
    if (tud_cdc_n_available(1)) {
        uint32_t i;

        uint32_t count = tud_cdc_n_read(1, buf, sizeof(buf));
        if (count > 0) {
            swap_case(count, buf);
            for (i = 0; i < count; i++) {
                putchar(buf[i]);
            }
        }
    }
    int ch = getchar_timeout_us(0);
    if (ch != PICO_ERROR_TIMEOUT) {
        buf[0] = ch;
        swap_case(1, buf);
        tud_cdc_n_write_char(1, buf[0]);
        tud_cdc_n_write_flush(1);
    }
}

int main(void) {
    stdio_init_all();
    pico_led_init();
    while(true) {
        led_task();
        cdc_task();
    }
}
