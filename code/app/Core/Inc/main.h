/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern volatile uint8_t switch_status;
extern uint8_t i2c_address[1];
extern volatile uint8_t is_irq_enable;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_K1_Pin GPIO_PIN_0
#define SW_K1_GPIO_Port GPIOA
#define SW_K1_EXTI_IRQn EXTI0_1_IRQn
#define SW_K2_Pin GPIO_PIN_1
#define SW_K2_GPIO_Port GPIOA
#define SW_K2_EXTI_IRQn EXTI0_1_IRQn
#define SW_K3_Pin GPIO_PIN_5
#define SW_K3_GPIO_Port GPIOA
#define SW_K3_EXTI_IRQn EXTI4_15_IRQn
#define SW_K4_Pin GPIO_PIN_6
#define SW_K4_GPIO_Port GPIOA
#define SW_K4_EXTI_IRQn EXTI4_15_IRQn
#define SW_K5_Pin GPIO_PIN_7
#define SW_K5_GPIO_Port GPIOA
#define SW_K5_EXTI_IRQn EXTI4_15_IRQn
#define SW_K8_Pin GPIO_PIN_8
#define SW_K8_GPIO_Port GPIOA
#define SW_K8_EXTI_IRQn EXTI4_15_IRQn
#define SW_K6_Pin GPIO_PIN_15
#define SW_K6_GPIO_Port GPIOA
#define SW_K6_EXTI_IRQn EXTI4_15_IRQn
#define SW_K7_Pin GPIO_PIN_3
#define SW_K7_GPIO_Port GPIOB
#define SW_K7_EXTI_IRQn EXTI2_3_IRQn
#define RGB_Pin GPIO_PIN_5
#define RGB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
