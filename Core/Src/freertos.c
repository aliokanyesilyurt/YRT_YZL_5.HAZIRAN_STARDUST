/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "functions.h"
#include "settings.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern bmi088_t imu;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for canTx */
osThreadId_t canTxHandle;
const osThreadAttr_t canTx_attributes = {
  .name = "canTx",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for stateGorev */
osThreadId_t stateGorevHandle;
const osThreadAttr_t stateGorev_attributes = {
  .name = "stateGorev",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for veriMutex */
osMutexId_t veriMutexHandle;
const osMutexAttr_t veriMutex_attributes = {
  .name = "veriMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void canTxTask(void *argument);
void stateGorevTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of veriMutex */
  veriMutexHandle = osMutexNew(&veriMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of canTx */
  canTxHandle = osThreadNew(canTxTask, NULL, &canTx_attributes);

  /* creation of stateGorev */
  stateGorevHandle = osThreadNew(stateGorevTask, NULL, &stateGorev_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

	  CAN_set();
	  MS5611_Init(&hi2c3, 0);
	  i2c_driver_init_i2c2();
	  imu.bus.read     = i2c_read_wrapper;
	  imu.bus.write    = i2c_write_wrapper;
	  imu.bus.dev_addr = BMI088_ACC_I2C_ADDR;
	  bmi088_init(&imu);

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_canTxTask */
/**
* @brief Function implementing the canTx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_canTxTask */
void canTxTask(void *argument)
{
  /* USER CODE BEGIN canTxTask */
  /* Infinite loop */
  for(;;)
  {
	  CAN_telemetriGonder();
    osDelay(80);
  }
  /* USER CODE END canTxTask */
}

/* USER CODE BEGIN Header_stateGorevTask */
/**
* @brief Function implementing the stateGorev thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_stateGorevTask */
void stateGorevTask(void *argument)
{
  /* USER CODE BEGIN stateGorevTask */
	osDelay(500);
  /* Infinite loop */
  for(;;)
  {
	  veriOkuma();
	  hizHesaplama(ivmeToplam - 9.81f);
    osDelay(70);
  }
  /* USER CODE END stateGorevTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

