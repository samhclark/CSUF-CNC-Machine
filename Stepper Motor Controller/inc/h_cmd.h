#include <stdint.h>
#include "buffer.h"
#include "packet.h"
#include "stepper.h"

extern enum H {H0,H1,H2,H3,H29,H30} H;

typedef struct H_CMD {
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t i;
	int32_t j;
	int32_t k;
	int32_t p;
	int8_t CMD_seq;
	enum H type;
} H_CMD;

void H_CMD_Clear(H_CMD * cmd);

typedef struct H_CMD_Queue {
	int capacity;			// max capacity of queue
	int size;					// current number of elements
	int front;				// index of first element
	int rear;					// index of last element
	H_CMD * H_Commands[BUFFER_SIZE];	// array of Coord elements
} H_CMD_Queue;

void H_CMD_Queue_Init(H_CMD_Queue * buffer);
int H_CMD_Queue_Push(H_CMD_Queue * buffer, H_CMD * newCommand);
H_CMD * H_CMD_Queue_Pop(H_CMD_Queue * buffer);
void extractNextPacket(H_CMD_Queue * H_Buffer);
