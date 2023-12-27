/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VC_CONTROL_Pin GPIO_PIN_0
#define VC_CONTROL_GPIO_Port GPIOC
#define DIAG0_SWN_Pin GPIO_PIN_2
#define DIAG0_SWN_GPIO_Port GPIOC
#define DIAG1_SWP_Pin GPIO_PIN_3
#define DIAG1_SWP_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define DRV_ENN_Pin GPIO_PIN_5
#define DRV_ENN_GPIO_Port GPIOC
#define DIP_1_Pin GPIO_PIN_10
#define DIP_1_GPIO_Port GPIOB
#define DIP_2_Pin GPIO_PIN_11
#define DIP_2_GPIO_Port GPIOB
#define DIP_3_Pin GPIO_PIN_12
#define DIP_3_GPIO_Port GPIOB
#define DIP_4_Pin GPIO_PIN_13
#define DIP_4_GPIO_Port GPIOB
#define DIP_5_Pin GPIO_PIN_14
#define DIP_5_GPIO_Port GPIOB
#define DIP_6_Pin GPIO_PIN_15
#define DIP_6_GPIO_Port GPIOB
#define DIP_7_Pin GPIO_PIN_8
#define DIP_7_GPIO_Port GPIOC
#define DIP_8_Pin GPIO_PIN_9
#define DIP_8_GPIO_Port GPIOC
#define STEP_Pin GPIO_PIN_8
#define STEP_GPIO_Port GPIOA
#define DIR_Pin GPIO_PIN_9
#define DIR_GPIO_Port GPIOA
#define SPI_MODE_Pin GPIO_PIN_11
#define SPI_MODE_GPIO_Port GPIOA
#define SD_MODE_Pin GPIO_PIN_12
#define SD_MODE_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

#define ROUTER_ID (127)					// CAN ID OF ROUTER DEVICE SHOULD BE MY_CAN_ID + 10

#define JOINT_N 2

#define USE_ENCODER 1
#define USE_FREEETOS 1
#define NODIPSWITCH //redefine if needed

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
