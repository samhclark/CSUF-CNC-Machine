#include <stdint.h>
#include "buffer.h"
#include "stm32f7xx_hal.h"

extern uint8_t Tx_Pkt_Seq;
extern UART_HandleTypeDef huart5;

typedef struct Pkt {
	uint8_t data[6];
	int error;
} Pkt;

typedef struct Pkt_Queue {
	int capacity;
	int size;
	int front;
	int rear;
	Pkt * Packets[BUFFER_SIZE];
} Pkt_Queue;

extern Pkt_Queue * Pkt_buffer;

void Pkt_Queue_Init(Pkt_Queue * buffer);
int Pkt_Queue_Push(Pkt * newPacket);
Pkt * Pkt_Queue_Pop(void);
int Tx_CurrentPos(uint32_t x, uint32_t y, uint32_t z);
