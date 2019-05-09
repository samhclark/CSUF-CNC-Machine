#include "h_cmd_move.h"

int move_H0(H_CMD * cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper) {
	////debug_printf("H0 command");
	// if H0 code has negatives return an error
	if (cmd->x < 0 || cmd->y < 0 || cmd->z < 0) return -1;
	// first move gantry head to new z position
	if (currentPos->z < cmd->z) {
		for (int i = currentPos->z; i < cmd->z; i++) {
			zStepperInc(zStepper);
			currentPos->z++;
			////debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	} else if (currentPos->z > 	cmd->z) {
		for (int i = currentPos->z; i > cmd->z; i--) {
			zStepperDec(zStepper);
			currentPos->z--;
			////debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	}
	// next move x position of gantry head to desired position
	if (currentPos->x < cmd->x) {			// moving head in positive x direction
		for (int i = currentPos->x; i < cmd->x; i++) {
			xStepperInc(xStepper);
			currentPos->x++;
			////debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	} else if (currentPos->x > cmd->x) {	// moving head in negative x direction
		for (int i = currentPos->x; i > cmd->x; i--) {
			xStepperDec(xStepper);
			currentPos->x--;
			////debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	}
	// next move y position of gantry head to desired position
	if (currentPos->y < cmd->y) {			// moving head in positive y direction
		for (int i = currentPos->y; i < cmd->y; i++) {
			yStepperInc(yStepper);
			currentPos->y++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	} else if (currentPos->y > cmd->y) {	// moving head in negative y direction
		for (int i = currentPos->y; i > cmd->y; i--) {
			yStepperDec(yStepper);
			currentPos->y--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H0_Delay();
		}
	}
	return 0;
}

int move_H1(H_CMD *cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper) {
	// first move gantry head down if its up
	if (currentPos->z > cmd->z) {
		for (int i = currentPos->z; i > cmd->z; i--) {
			zStepperDec(zStepper);
			currentPos->z--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->z < cmd->z) {
		for (int i = currentPos->z; i < cmd->z; i++) {
			zStepperInc(zStepper);
			currentPos->z++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}

	// find deltas
	int32_t delX = cmd->x - currentPos->x;
	int32_t delY = cmd->y - currentPos->y;

	int32_t x1,y1,octant;

	// determines octant
	if ((delX>=0)	& (delY>=0))	{octant=(abs(delY)>abs(delX))?1:0;}
	if ((delX<0)	& (delY>=0))	{octant=(abs(delY)>abs(delX))?2:3;}
	if ((delX<0)	& (delY<0))	{octant=(abs(delY)>abs(delX))?5:4;}
	if ((delX>=0)	& (delY<0))	{octant=(abs(delY)>abs(delX))?6:7;}


	// maps current octant to first octant
	switch(octant){
		case 0:{x1=delX; y1=delY; break;}
		case 1:{x1=delY; y1=delX; break;}
		case 2:{x1=delY; y1=-delX; break;}
		case 3:{x1=-delX; y1=delY; break;}
		case 4:{x1=-delX; y1=-delY; break;}
		case 5:{x1=-delY; y1=-delX; break;}
		case 6:{x1=-delY; y1=delX; break;}
		case 7:{x1=delX; y1=-delY; break;}
	}

	// Bresenham's line algorithm for octant 1
	// for octant 1 x is the incremented variable, so y is selectively incremented
	int DelY2=(y1<<1);					// 2DelY = 2*delY
	int DelX2=(x1<<1);					// 2DelX= 2*delX
	int DelXErrY2=-x1;					// 2DelXErrorY = -DelX
	int y=0;										// start value for non dominant axis
	for (int x=1; x<=x1; x++){
		DelXErrY2+=DelY2;					// error  = error + 2*delY
		if (DelXErrY2>=0){				// midpoint equals E/2+offset=0
			DelXErrY2-=DelX2;				//
			// map y back to correct octant
			switch(octant){
				case 0:{yStepperInc(yStepper); currentPos->y++; break;}
				case 1:{xStepperInc(xStepper); currentPos->x++; break;}
				case 2:{xStepperDec(xStepper); currentPos->x--; break;}
				case 3:{yStepperInc(yStepper); currentPos->y++; break;}
				case 4:{yStepperDec(yStepper); currentPos->y--; break;}
				case 5:{xStepperDec(xStepper); currentPos->x--; break;}
				case 6:{xStepperInc(xStepper); currentPos->x++; break;}
				case 7:{yStepperDec(yStepper); currentPos->y--; break;}
			}
			////debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			y++;
		}
		// map x back to correct octant
		switch(octant){
			case 0:{xStepperInc(xStepper); currentPos->x++; break;}
			case 1:{yStepperInc(yStepper); currentPos->y++; break;}
			case 2:{yStepperInc(yStepper); currentPos->y++; break;}
			case 3:{xStepperDec(xStepper); currentPos->x--; break;}
			case 4:{xStepperDec(xStepper); currentPos->x--; break;}
			case 5:{yStepperDec(yStepper); currentPos->y--; break;}
			case 6:{yStepperDec(yStepper); currentPos->y--; break;}
			case 7:{xStepperInc(xStepper); currentPos->x++; break;}
		}
		//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
		Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
		H123_Delay();
	}
	// checks to see if current x is off by one or two steps, moves it back into alignment
	if (currentPos->x < cmd->x) {
		for (int i = currentPos->x; i < cmd->x; i++) {
			xStepperInc(xStepper);
			currentPos->x++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->x > cmd->x) {
		for (int i = currentPos->x; i > cmd->x; i--) {
			xStepperDec(xStepper);
			currentPos->x--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	// checks to see if current y is off by one or two steps, moves it back into alignment
	if (currentPos->y < cmd->y) {
		for (int i = currentPos->y; i < cmd->y; i++) {
			yStepperInc(yStepper);
			currentPos->y++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->y > cmd->y) {
		for (int i = currentPos->y; i > cmd->y; i--) {
			yStepperDec(yStepper);
			currentPos->y--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	return 0;
}

int move_H2(H_CMD *cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper) {
	// first move gantry head down if its up
	if (currentPos->z > cmd->z) {
		for (int i = currentPos->z; i > cmd->z; i--) {
			zStepperDec(zStepper);
			currentPos->z--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->z < cmd->z) {
		for (int i = currentPos->z; i < cmd->z; i++) {
			zStepperInc(zStepper);
			currentPos->z++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	int x1 = currentPos->x;
	int y1 = currentPos->y;
	int x2 = cmd->x;
	int y2 = cmd->y;
	int i = cmd->i;
	int j = cmd->j;
	float32_t r;

	arm_sqrt_f32((x2-i)*(x2-i) + (y2-j)*(y2-j),&r);

	// get relative x coordinates: m
	int m1 = x1 - i;
	int m2 = x2 - i;

	// determine quadrant
	int quad1,quad2 = 0;

	if (x1 > i && y1 >= j) 		quad1 = 1;
	else if (x1 <= i && y1 > j)	quad1 = 2;
	else if (x1 < i && y1 <= j)	quad1 = 3;
	else if (x1 >= i && y1 < j)	quad1 = 4;

	if (x2 > i && y2 >= j)		quad2 = 1;
	else if (x2 <= i && y2 > j)	quad2 = 2;
	else if (x2 < i && y2 <= j)	quad2 = 3;
	else if (x2 >= i && y2 < j)	quad2 = 4;

	float32_t startAngle = acos(m1/r);
	// if angle lands in a quadrant outside of arc cosine's domain
	if (quad1 == 3 || quad1 == 4) startAngle = 2*PI - startAngle;

	float32_t endAngle = acos(m2/r);
	// if angle lands in a quadrant outside of arc cosine's domain
	if (quad2 == 3 || quad2 == 4) endAngle = 2*PI - endAngle;

	float32_t incAngle = 1.414f/r;
	// if startAngle is less than the endAngle in such a way
	// that k will cross the 0 degree line, if so we need to break
	// it up into two sections, from startAngle->0 then from
	// 2*pi->endAngle
	if (quad2 > quad1 || (quad1 == quad2 && startAngle < endAngle)) {
		float32_t k = startAngle;
		while (k > 0) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k - incAngle;
		}
		k = 2*PI;
		while (k >= endAngle) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k - incAngle;
		}
	// otherwise just go from startAngle to endAngle
	} else {
		float32_t k = startAngle;
		while (k >= endAngle) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k - incAngle;
		}
	}
	// checks to see if current x is off by one or two steps, moves it back into alignment
	if (currentPos->x < cmd->x) {
		for (int i = currentPos->x; i < cmd->x; i++) {
			xStepperInc(xStepper);
			currentPos->x++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->x > cmd->x) {
		for (int i = currentPos->x; i > cmd->x; i--) {
			xStepperDec(xStepper);
			currentPos->x--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	// checks to see if current y is off by one or two steps, moves it back into alignment
	if (currentPos->y < cmd->y) {
		for (int i = currentPos->y; i < cmd->y; i++) {
			yStepperInc(yStepper);
			currentPos->y++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->y > cmd->y) {
		for (int i = currentPos->y; i > cmd->y; i--) {
			yStepperDec(yStepper);
			currentPos->y--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	return 0;
}

int move_H3(H_CMD *cmd, H_CMD * currentPos, Stepper * zStepper,
	Stepper * xStepper, Stepper * yStepper) {
	// first move gantry head down if its up
	if (currentPos->z > cmd->z) {
		for (int i = currentPos->z; i > cmd->z; i--) {
			zStepperDec(zStepper);
			currentPos->z--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->z < cmd->z) {
		for (int i = currentPos->z; i < cmd->z; i++) {
			zStepperInc(zStepper);
			currentPos->z++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	int x1 = currentPos->x;
	int y1 = currentPos->y;
	int x2 = cmd->x;
	int y2 = cmd->y;
	int i = cmd->i;
	int j = cmd->j;
	float32_t r;

	arm_sqrt_f32((x2-i)*(x2-i) + (y2-j)*(y2-j),&r);

	// get relative x coordinates: m
	int m1 = x1 - i;
	int m2 = x2 - i;

	// determine quadrant
	int quad1,quad2 = 0;

	if (x1 > i && y1 >= j) 			quad1 = 1;
	else if (x1 <= i && y1 > j)	quad1 = 2;
	else if (x1 < i && y1 <= j)	quad1 = 3;
	else if (x1 >= i && y1 < j)	quad1 = 4;

	if (x2 > i && y2 >= j)			quad2 = 1;
	else if (x2 <= i && y2 > j)	quad2 = 2;
	else if (x2 < i && y2 <= j)	quad2 = 3;
	else if (x2 >= i && y2 < j)	quad2 = 4;

	float32_t startAngle = acos(m1/r);
	////debug_printf("Start angle: %f, Quadrant: %d\n",startAngle,quad1);
	// if angle lands in a quadrant outside of arc cosine's domain
	if ((quad1 == 3 || quad1 == 4) || (quad1 == 2 && startAngle == 0.000000f)) startAngle = 2*PI - startAngle;

	float32_t endAngle = acos(m2/r);
	////debug_printf("End angle: %f, Quadrant: %d\n",startAngle,quad2);
	// if angle lands in a quadrant outside of arc cosine's domain
	if (quad2 == 3 || quad2 == 4) endAngle = 2*PI - endAngle;

	float32_t incAngle = 1.414f/r;
	////debug_printf("Start: %f, End: %f, Inc: %f, R: %f\n",startAngle,endAngle,incAngle,r);
	// if startAngle is greater than the endAngle in such a way
	// that k will cross the 0 degree line, if so we need to break
	// it up into two sections, from startAngle->2*pi then from
	// 0->endAngle
	if (quad2 < quad1 || (quad1 == quad2 && startAngle > endAngle)) {
		float32_t k = startAngle;
		while (k < 2*PI) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			////debug_printf("Starting X: %d, Y: %d",x,y);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k + incAngle;
		}
		k = 0;
		while (k <= endAngle) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k + incAngle;
		}
	} else {
		float32_t k = startAngle;
		while (k <= endAngle) {
			uint32_t x = (int)round(r*cos(k) + i);
			uint32_t y = (int)round(r*sin(k) + j);
			////debug_printf("New X: %d, New Y: %d \n",x,y);
			// MOVE STEPPERS
			// if the difference between two calculated coordinates is greater than
			// 1 then use the H1 algorithm to connect the two points
			// this allows more flexible increment angle calculation
			if ((abs((int32_t)(currentPos->x - x)) > 1) || (abs((int32_t)(currentPos->y - y)) > 1)) {
				connectPointsLinear(x,y,currentPos,xStepper,yStepper,zStepper);
			}
			// otherwise just make one step in the correct direction
			else {
				if (currentPos->x < x) {
					xStepperInc(xStepper);
					currentPos->x++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->x > x) {
					xStepperDec(xStepper);
					currentPos->x--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				if (currentPos->y < y) {
					yStepperInc(yStepper);
					currentPos->y++;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
				else if (currentPos->y > y) {
					yStepperDec(yStepper);
					currentPos->y--;
					//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
				}
			}
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
			k = k + incAngle;
		}
	}
	// checks to see if current x is off by one or two steps, moves it back into alignment
	if (currentPos->x < cmd->x) {
		for (int i = currentPos->x; i < cmd->x; i++) {
			xStepperInc(xStepper);
			currentPos->x++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->x > cmd->x) {
		for (int i = currentPos->x; i > cmd->x; i--) {
			xStepperDec(xStepper);
			currentPos->x--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	// checks to see if current y is off by one or two steps, moves it back into alignment
	if (currentPos->y < cmd->y) {
		for (int i = currentPos->y; i < cmd->y; i++) {
			yStepperInc(yStepper);
			currentPos->y++;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	} else if (currentPos->y > cmd->y) {
		for (int i = currentPos->y; i > cmd->y; i--) {
			yStepperDec(yStepper);
			currentPos->y--;
			//debug_printf(("%d %d %d\n", currentPos->x,currentPos->y,currentPos->z));
			Tx_CurrentPos(currentPos->x,currentPos->y,currentPos->z);
			H123_Delay();
		}
	}
	return 0;
}

int connectPointsLinear(uint32_t x, uint32_t y, H_CMD * currentPos, Stepper * xStepper, Stepper * yStepper, Stepper * zStepper) {
	H_CMD * temp = (H_CMD *)malloc(sizeof(H_CMD));
	temp->x = x;
	temp->y = y;
	temp->z = currentPos->z;
	move_H1(temp,currentPos,zStepper,xStepper,yStepper);
	free(temp);
	temp = NULL;
	return 0;
}

int resetPosition(H_CMD * currentPos, Stepper * xStepper, Stepper * yStepper, Stepper * zStepper) {
	H_CMD * reset = (H_CMD *)malloc(sizeof(H_CMD));
	reset->x = 1;
	reset->y = 1;
	reset->z = 1;
	move_H0(reset,currentPos,xStepper,yStepper,zStepper);
	motorsRelax(xStepper,yStepper,zStepper);
	return 0;
}
