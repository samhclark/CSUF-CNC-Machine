#include <stdlib.h>

typedef struct Stepper {
	int stepPos;
} Stepper;

void xStepper_Init(Stepper * stepper);
void yStepper_Init(Stepper * stepper);
void zStepper_Init(Stepper * stepper);
void xStepper_Relax(Stepper * stepper);
void yStepper_Relax(Stepper * stepper);
void zStepper_Relax(Stepper * stepper);
void xStepperInc(Stepper * zStepper);
void xStepperDec(Stepper * zStepper);
void yStepperInc(Stepper * zStepper);
void yStepperDec(Stepper * zStepper);
void zStepperInc(Stepper * zStepper);
void zStepperDec(Stepper * zStepper);
int motorsRelax(Stepper * xStepper, Stepper * yStepper, Stepper * zStepper);
