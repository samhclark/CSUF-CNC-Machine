#include <stdlib.h>
#include "h_cmd.h"

void H_CMD_Clear(H_CMD * cmd) {
	cmd->x = 0;
	cmd->y = 0;
	cmd->z = 0;
	cmd->i = 0;
	cmd->j = 0;
	cmd->k = 0;
	cmd->p = 0;
	cmd->type = H0;
	cmd->CMD_seq = 0;
}

void H_CMD_Queue_Init(H_CMD_Queue * buffer) {
	buffer->capacity = BUFFER_SIZE;
	buffer->size = 0;
	buffer->front = 0;
	buffer->rear = 0;
}

int H_CMD_Queue_Push(H_CMD_Queue * buffer, H_CMD * newCommand) {
	if (buffer->capacity == buffer->size) return -1; // -1 means the buffer is full
	else if ((buffer->size > buffer->capacity) ||
		((buffer->size == 0 || buffer->size == 1) !=
		(buffer->front == buffer->rear)))
		return -2; // -2 means there's a buffer error
	else {
		if (buffer->size == 0) buffer->H_Commands[buffer->rear] = newCommand;
		else buffer->H_Commands[(buffer->rear + 1) % BUFFER_SIZE] = newCommand;
		buffer->size++;
		if (buffer->size == 1) return 0;
		buffer->rear++;
		if (buffer->rear == buffer->capacity) buffer->rear = 0;
	}
	return 0; // 0 means correct funcitoning of buffer and H_CMD was properly
						// pushed into it
}

H_CMD * H_CMD_Queue_Pop(H_CMD_Queue * buffer) {
	H_CMD * temp;
	if (buffer->size == 0) {
		temp->x = -1; // -1 in H_CMD members means buffer is empty
		temp->y = -1;
		temp->z = -1;
		return temp;
	} else if ((buffer->size > buffer->capacity) ||
		((buffer->size == 0 || buffer->size == 1) !=
		(buffer->front == buffer->rear))) {
		temp->x = -2; // -2 means error with Queue
		temp->y = -2;
		temp->z = -2;
		return temp;
	}	else {
		buffer->size--;
		// if front and rear are already the same just return
		if (buffer->size == 0) return buffer->H_Commands[buffer->front];
		buffer->front++;
		if (buffer->front == buffer->capacity) buffer->front = 0;
		return buffer->H_Commands[buffer->front - 1];
	}
}

// this function takes only one packet from the packet buffer and either appends
// it values to the H_CMD that its parameters are a part of, or starts a new
// H_CMD to add to the H_Buffer. This function only processes 1 packet at a time
void extractNextPacket(H_CMD_Queue * H_Buffer) {
	if (Pkt_buffer->size == 0) return; // there's nothing in the buffer to process
	Pkt * currentPkt = Pkt_Queue_Pop();
	uint8_t current_pktCmdSeq = currentPkt->data[1] >> 4;
	if (H_Buffer->size == 0 || H_Buffer->H_Commands[H_Buffer->rear]->CMD_seq !=
		current_pktCmdSeq) {
		H_CMD * newH_CMD = (H_CMD *)malloc(sizeof(H_CMD)); // creates new H_CMD
		H_CMD_Clear(newH_CMD);

		// determine what H-code it is
		uint8_t H_Type = currentPkt->data[0] & 15; // keeps only the H-code component
		if (H_Type == 0) newH_CMD->type = H0; // sets the H-code type based off of
																					// what is held in packet
		else if (H_Type == 1) newH_CMD->type = H1;
		else if (H_Type == 2) newH_CMD->type = H2;
		else if (H_Type == 3) newH_CMD->type = H3;
		else if (H_Type == 8) newH_CMD->type = H29;
		else if (H_Type == 15) newH_CMD->type = H30;

		// set command sequence of H_CMD
		newH_CMD->CMD_seq = current_pktCmdSeq;

		// sets parameter of current packet
		uint8_t currentParam = currentPkt->data[1] & 15;
		uint32_t currentCoord = currentPkt->data[2] << 24;
		currentCoord |= currentPkt->data[3] << 16;
		currentCoord |= currentPkt->data[4] << 8;
		currentCoord |= currentPkt->data[5];
		if (currentParam == 0) newH_CMD->x = currentCoord;
		else if (currentParam == 1) newH_CMD->y = currentCoord;
		else if (currentParam == 2) newH_CMD->z = currentCoord;
		else if (currentParam == 3) newH_CMD->i = currentCoord;
		else if (currentParam == 4) newH_CMD->j = currentCoord;
		else if (currentParam == 5) newH_CMD->k = currentCoord;
		else if (currentParam == 6) newH_CMD->p = currentCoord;

		// push new H_CMD to H_Buffer
		H_CMD_Queue_Push(H_Buffer,newH_CMD);

	// if the current packet has a parameter for the last H_CMD in the H_Buffer
	} else if (H_Buffer->H_Commands[H_Buffer->rear]->CMD_seq == current_pktCmdSeq) {
		H_CMD * currentH_CMD = H_Buffer->H_Commands[H_Buffer->rear];

		// sets parameter of current packet
		uint8_t currentParam = currentPkt->data[1] & 15;
		uint32_t currentCoord = currentPkt->data[2] << 24;
		currentCoord |= currentPkt->data[3] << 16;
		currentCoord |= currentPkt->data[4] << 8;
		currentCoord |= currentPkt->data[5];
		if (currentParam == 0) currentH_CMD->x = currentCoord;
		else if (currentParam == 1) currentH_CMD->y = currentCoord;
		else if (currentParam == 2) currentH_CMD->z = currentCoord;
		else if (currentParam == 3) currentH_CMD->i = currentCoord;
		else if (currentParam == 4) currentH_CMD->j = currentCoord;
		else if (currentParam == 5) currentH_CMD->k = currentCoord;
	}
	free(currentPkt);
	currentPkt = NULL;
}
