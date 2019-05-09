#include "h_cmd.h"
#include "math.h"
#include "stm32f7xx_hal.h"
#include "arm_math.h"
#include "nucleo_delay.h"

// ----------------------------------------------
// DEBUG MODE SETTING, IF DEBUG MODE IS 1
// THEN COMPILER WILL KEEP //debug_printf STATEMENTS
// OTHERWISE COMPILER WILL IGNORE THEM
#define DEBUG 0

#if DEBUG
	#define debug_printf(a) printf a
#else
	#define debug_printf(a) (void)0
#endif
// ----------------------------------------------


int move_H0(H_CMD * cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper);
int move_H1(H_CMD * cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper);
int move_H2(H_CMD * cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper);
int move_H3(H_CMD * cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper);
int connectPointsLinear(uint32_t x, uint32_t y, 
	H_CMD * currentPos, Stepper * xStepper, Stepper * yStepper, 
	Stepper * zStepper);
int resetPosition(H_CMD * currentPos, Stepper * xStepper, 
	Stepper * yStepper, Stepper * zStepper);
