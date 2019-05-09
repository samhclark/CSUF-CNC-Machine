#define MAX_STEPS 4000

#include "stepper.h"
#include "stm32f7xx_hal.h"

void xStepper_Init(Stepper * stepper) {
	stepper->stepPos = 0;
	HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4),GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_7), GPIO_PIN_SET);
}

void yStepper_Init(Stepper * stepper) {
	stepper->stepPos = 0;
	HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6),GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_2), GPIO_PIN_SET);
}

void zStepper_Init(Stepper * stepper) {
	stepper->stepPos = 0;
	HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15),GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_10), GPIO_PIN_SET);
}

void xStepper_Relax(Stepper * stepper) {
	stepper->stepPos = -1;
	HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4),GPIO_PIN_RESET);
}

void yStepper_Relax(Stepper * stepper) {
	stepper->stepPos = -1;	
	HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6),GPIO_PIN_RESET);
}

void zStepper_Relax(Stepper * stepper) {
	stepper->stepPos = -1;
	HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15),GPIO_PIN_RESET);
}

void xStepperInc(Stepper * xStepper) {
	if (xStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_6 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_4 | GPIO_PIN_7) , GPIO_PIN_SET);
		xStepper->stepPos = 1;
	} else if (xStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_7 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_4 | GPIO_PIN_6), GPIO_PIN_SET);	
		xStepper->stepPos = 2;
	} else if (xStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_7 | GPIO_PIN_4),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_5 | GPIO_PIN_6), GPIO_PIN_SET);
		xStepper->stepPos = 3;
	} else if (xStepper->stepPos == 3 || xStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_6 | GPIO_PIN_4),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_5 | GPIO_PIN_7), GPIO_PIN_SET);
		xStepper->stepPos = 0;
	}
}

void xStepperDec(Stepper * xStepper) {
	if (xStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_6 | GPIO_PIN_4),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_5 | GPIO_PIN_7), GPIO_PIN_SET);
		xStepper->stepPos = 3;
	} else if (xStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_6 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_4 | GPIO_PIN_7), GPIO_PIN_SET);
		xStepper->stepPos = 0;
	} else if (xStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_7 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_4 | GPIO_PIN_6) , GPIO_PIN_SET);
		xStepper->stepPos = 1;
	} else if (xStepper->stepPos == 3 || xStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOD,(GPIO_PIN_7 | GPIO_PIN_4),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, (GPIO_PIN_5 | GPIO_PIN_6), GPIO_PIN_SET);
		xStepper->stepPos = 2;
	}
}

void yStepperInc(Stepper * yStepper) {
	if (yStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_4 | GPIO_PIN_6),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_2 | GPIO_PIN_5), GPIO_PIN_SET);
		yStepper->stepPos = 1;
	} else if (yStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_2 | GPIO_PIN_6),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_4 | GPIO_PIN_5), GPIO_PIN_SET);
		yStepper->stepPos = 2;
	} else if (yStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_2 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_4 | GPIO_PIN_6), GPIO_PIN_SET);
		yStepper->stepPos = 3;
	} else if (yStepper->stepPos == 3 || yStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_4 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_2 | GPIO_PIN_6), GPIO_PIN_SET);
		yStepper->stepPos = 0;
	}
}

void yStepperDec(Stepper * yStepper) {
	if (yStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_2 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_4 | GPIO_PIN_6), GPIO_PIN_SET);
		yStepper->stepPos = 3;
	} else if (yStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_4 | GPIO_PIN_5),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_2 | GPIO_PIN_6), GPIO_PIN_SET);
		yStepper->stepPos = 0;
	} else if (yStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_4 | GPIO_PIN_6),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_2 | GPIO_PIN_5), GPIO_PIN_SET);
		yStepper->stepPos = 1;
	} else if (yStepper->stepPos == 3 || yStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_2 | GPIO_PIN_6),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_4 | GPIO_PIN_5), GPIO_PIN_SET);
		yStepper->stepPos = 2;
	}
}

void zStepperDec(Stepper * zStepper) {
	if (zStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_12 | GPIO_PIN_15),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_10 | GPIO_PIN_14), GPIO_PIN_SET);
		zStepper->stepPos = 1;
	} else if (zStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_10 | GPIO_PIN_15),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_12 | GPIO_PIN_14), GPIO_PIN_SET);
		zStepper->stepPos = 2;
	} else if (zStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_10 | GPIO_PIN_14),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_12 | GPIO_PIN_15) , GPIO_PIN_SET);
		zStepper->stepPos = 3;
	} else if (zStepper->stepPos == 3 || zStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_12 | GPIO_PIN_14),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_10 | GPIO_PIN_15), GPIO_PIN_SET);
		zStepper->stepPos = 0;
	}
}

void zStepperInc(Stepper * zStepper) {
	if (zStepper->stepPos == 0) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_10 | GPIO_PIN_14),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_12 | GPIO_PIN_15) , GPIO_PIN_SET);
		zStepper->stepPos = 3;
	} else if (zStepper->stepPos == 1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_12 | GPIO_PIN_14),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_10 | GPIO_PIN_15), GPIO_PIN_SET);
		zStepper->stepPos = 0;
	} else if (zStepper->stepPos == 2) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_12 | GPIO_PIN_15),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_10 | GPIO_PIN_14), GPIO_PIN_SET);
		zStepper->stepPos = 1;
	} else if (zStepper->stepPos == 3 || zStepper->stepPos == -1) {
		HAL_GPIO_WritePin(GPIOE,(GPIO_PIN_10 | GPIO_PIN_15),GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, (GPIO_PIN_12 | GPIO_PIN_14), GPIO_PIN_SET);
		zStepper->stepPos = 2;
	}
}

int motorsRelax(Stepper * xStepper, Stepper * yStepper, Stepper * zStepper) {
	xStepper_Relax(xStepper);
	yStepper_Relax(yStepper);
	zStepper_Relax(zStepper);
	return 0;
}
