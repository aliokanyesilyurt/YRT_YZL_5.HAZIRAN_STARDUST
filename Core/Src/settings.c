#include <settings.h>
#include <string.h>

extern CAN_HandleTypeDef hcan1;
osMessageQueueId_t canRxQueueHandle;
uint32_t canTx_OK = 0;

void CAN_set(void){

	CAN_FilterTypeDef canfilterconfig = {0};
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 0;
	canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canfilterconfig.FilterIdHigh = 0x0000;
	canfilterconfig.FilterIdLow = 0x0000;
	canfilterconfig.FilterMaskIdHigh = 0x0000;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1,
	    CAN_IT_RX_FIFO0_MSG_PENDING |
	    CAN_IT_TX_MAILBOX_EMPTY);
	canRxQueueHandle = osMessageQueueNew(10, sizeof(CAN_Msg_t), NULL);
}

void CAN_sendData(uint32_t id, uint8_t *data, uint8_t length){
	CAN_TxHeaderTypeDef TxHeader = {0};
	uint32_t TxMailbox;

	TxHeader.StdId = id;
	TxHeader.ExtId = 0;
 	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = length;

	uint32_t baslangicTick = HAL_GetTick();
		while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
			if ((HAL_GetTick() - baslangicTick) > 5) return;
		}

	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    if (hcan->Instance == CAN1)
    {
        CAN_Msg_t newMsg;
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &newMsg.header, (uint8_t*)newMsg.data);
        osMessageQueuePut(canRxQueueHandle, &newMsg, 0, 0);
    }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) { canTx_OK++; }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) { canTx_OK++; }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) { canTx_OK++; }
