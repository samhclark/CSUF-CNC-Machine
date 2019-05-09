#include "stm32f7xx_hal.h"
#include <stdint.h>

#define H0_DELAY 5
#define H123_DELAY 10

int get_microseconds(void);
void Nucleo_Delay_US(uint16_t micros);
void H0_Delay(void);
void H123_Delay(void);
