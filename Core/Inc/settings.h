#ifndef SETTINGS_H
#define SETTINGS_H

#include "main.h"
#include <stdlib.h>
#include "cmsis_os.h"
#include "functions.h"

#define TX_BASLAT 0x07
extern uint8_t txAktif;

typedef struct{
	CAN_RxHeaderTypeDef header;
	uint8_t data[8];
}CAN_Msg_t;

void CAN_set(void);
void CAN_sendData(uint32_t id, uint8_t *data, uint8_t length);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

extern osMessageQueueId_t canRxQueueHandle;
extern CAN_HandleTypeDef hcan1;
#endif
