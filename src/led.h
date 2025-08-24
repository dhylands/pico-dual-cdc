#pragma once

int pico_led_init(void);
void pico_set_led(bool len_on);
void led_task(void);