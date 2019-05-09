#include <stdlib.h>
#include "packet.h"

// call this function to initialize the packet queue
void Pkt_Queue_Init(Pkt_Queue * buffer) {
	buffer->capacity = BUFFER_SIZE;
	buffer->size = 0;
	buffer->front = 0;
	buffer->rear = 0;
}

int Pkt_Queue_Push(Pkt * newPacket) {
	if (Pkt_buffer->capacity == Pkt_buffer->size) return -1; // -1 means the
															 // Pkt_buffer is full
	else if ((Pkt_buffer->size > Pkt_buffer->capacity) ||
		((Pkt_buffer->size == 0 || Pkt_buffer->size == 1) !=
		(Pkt_buffer->front == Pkt_buffer->rear)))
		return -2; // -2 means there's a Pkt_buffer error
	else {
		if (Pkt_buffer->size == 0) Pkt_buffer->Packets[Pkt_buffer->rear] = newPacket;
		else Pkt_buffer->Packets[(Pkt_buffer->rear + 1) % BUFFER_SIZE] = newPacket;
		Pkt_buffer->size++;
		if (Pkt_buffer->size == 1) return 0;
		Pkt_buffer->rear++;
		if (Pkt_buffer->rear == Pkt_buffer->capacity) Pkt_buffer->rear = 0;
	}
	return 0; // 0 means correct funcitoning of Pkt_buffer and packet was properly
	 		  // pushed into it
}

Pkt * Pkt_Queue_Pop(void) {
	Pkt * temp;
	if (Pkt_buffer->size == 0) {
		temp = (Pkt *)malloc(sizeof(Pkt)*2);
		temp->error = -1; // -1 in first byte of packet struct data members means
											// Pkt_buffer is empty
		return temp;
	} else if (
		(Pkt_buffer->size > Pkt_buffer->capacity) ||
		((Pkt_buffer->size == 0 || Pkt_buffer->size == 1) !=
		(Pkt_buffer->front == Pkt_buffer->rear))) {

		temp = (Pkt *)malloc(sizeof(Pkt)*2);
		temp->error = -2; // -2 means error with Queue
		return temp;
	}	else {
		Pkt_buffer->size--;
		// if front and rear are already the same just return
		if (Pkt_buffer->size == 0) return Pkt_buffer->Packets[Pkt_buffer->front];
		Pkt_buffer->front++;
		if (Pkt_buffer->front == Pkt_buffer->capacity) {
			Pkt_buffer->front = 0;
			return Pkt_buffer->Packets[Pkt_buffer->capacity - 1];
		}
		return Pkt_buffer->Packets[Pkt_buffer->front - 1];
	}
	
}

int Tx_CurrentPos(uint32_t x, uint32_t y, uint32_t z) {
	uint8_t Tx_Packet[6];
	// send X position via UART
	Tx_Packet[0] = Tx_Pkt_Seq << 4;
	Tx_Packet[0] |= 0xA;		// current position command
	Tx_Packet[1] = 0;			// x parameter
	Tx_Packet[2] = (x >> 24) & 0xFF;
	Tx_Packet[3] = (x >> 16) & 0xFF;
	Tx_Packet[4] = (x >> 8) & 0xFF;
	Tx_Packet[5] = x & 0xFF;
	HAL_UART_Transmit_IT(&huart5, (uint8_t *) Tx_Packet,6); // transmit x pos
	Tx_Pkt_Seq = (Tx_Pkt_Seq + 1) & 0xF;
	
	// send Y position via UART
	Tx_Packet[0] = Tx_Pkt_Seq << 4;
	Tx_Packet[1] = 1;			// y parameter
	Tx_Packet[2] = (y >> 24) & 0xFF;
	Tx_Packet[3] = (y >> 16) & 0xFF;
	Tx_Packet[4] = (y >> 8) & 0xFF;
	Tx_Packet[5] = y & 0xFF;
	HAL_UART_Transmit_IT(&huart5, (uint8_t *) Tx_Packet,6); // transmit y pos
	Tx_Pkt_Seq = (Tx_Pkt_Seq + 1) & 0xF;
	
	// send Z position via UART
	Tx_Packet[0] = Tx_Pkt_Seq << 4;
	Tx_Packet[1] = 2;			// z parameter
	Tx_Packet[2] = (z >> 24) & 0xFF;
	Tx_Packet[3] = (z >> 16) & 0xFF;
	Tx_Packet[4] = (z >> 8) & 0xFF;
	Tx_Packet[5] = z & 0xFF;
	HAL_UART_Transmit_IT(&huart5, (uint8_t *) Tx_Packet,6); // transmit z pos
	Tx_Pkt_Seq = (Tx_Pkt_Seq + 1) & 0xF;
	
	return 0;
}
