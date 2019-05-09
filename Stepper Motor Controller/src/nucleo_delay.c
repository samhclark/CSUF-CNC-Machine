#include "nucleo_delay.h"

// This funciton gets the current number of ticks in terms of microseconds since
// last system reset
int get_microseconds(void) {
	// get the number of system ticks per microsecond
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000; // returns the clock frequency of the system

	// place variables directly into registers to ensure fastest speed
	register uint32_t ms, cycle_cnt;
	do {
		ms = HAL_GetTick(); // get current ticks since startup in milliseconds
		cycle_cnt = SysTick->VAL; // gets current SysTick value
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

// This function delays the Nucleo board by a specified number of microseconds
void Nucleo_Delay_US(uint16_t micros) {
	uint32_t start = get_microseconds();
	while (get_microseconds()-start < (uint32_t) micros) {
		__asm("nop");
	}
}

void H0_Delay(void) {
	HAL_Delay(H0_DELAY);
}

void H123_Delay(void) {
	HAL_Delay(H123_DELAY);
}
