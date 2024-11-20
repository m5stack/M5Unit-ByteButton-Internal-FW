/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_tim3_ch2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
    NVIC_SystemReset();
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    NVIC_SystemReset();
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 and line 1 interrupts.
  */
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SW_K1_Pin);
  HAL_GPIO_EXTI_IRQHandler(SW_K2_Pin);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */
  if ((SW_K1_GPIO_Port->IDR&(SW_K1_Pin))>0) {
    switch_status |= (!!(SW_K1_GPIO_Port->IDR&(SW_K1_Pin)));
  }
  else {
    switch_status &= (~(1));
  }

  if ((SW_K2_GPIO_Port->IDR&(SW_K2_Pin))>0) {
    switch_status |= ((!!(SW_K2_GPIO_Port->IDR&(SW_K2_Pin))) << 1);
  }
  else {
    switch_status &= (~(1 << 1));
  }

  if (is_irq_enable)
    GPIOA->BRR = GPIO_PIN_13;
  /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 2 and line 3 interrupts.
  */
void EXTI2_3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_3_IRQn 0 */

  /* USER CODE END EXTI2_3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SW_K7_Pin);
  /* USER CODE BEGIN EXTI2_3_IRQn 1 */
  if ((SW_K7_GPIO_Port->IDR&(SW_K7_Pin))>0) {
    switch_status |= ((!!(SW_K7_GPIO_Port->IDR&(SW_K7_Pin))) << 6);
  }
  else {
    switch_status &= (~(1 << 6));
  }

  if (is_irq_enable)
    GPIOA->BRR = GPIO_PIN_13;  
  /* USER CODE END EXTI2_3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SW_K3_Pin);
  HAL_GPIO_EXTI_IRQHandler(SW_K4_Pin);
  HAL_GPIO_EXTI_IRQHandler(SW_K5_Pin);
  HAL_GPIO_EXTI_IRQHandler(SW_K8_Pin);
  HAL_GPIO_EXTI_IRQHandler(SW_K6_Pin);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
  if ((SW_K3_GPIO_Port->IDR&(SW_K3_Pin))>0) {
    switch_status |= ((!!(SW_K3_GPIO_Port->IDR&(SW_K3_Pin))) << 2);
  }
  else {
    switch_status &= (~(1 << 2));
  }

  if ((SW_K4_GPIO_Port->IDR&(SW_K4_Pin))>0) {
    switch_status |= ((!!(SW_K4_GPIO_Port->IDR&(SW_K4_Pin))) << 3);
  }
  else {
    switch_status &= (~(1 << 3));
  }  

  if ((SW_K5_GPIO_Port->IDR&(SW_K5_Pin))>0) {
    switch_status |= ((!!(SW_K5_GPIO_Port->IDR&(SW_K5_Pin))) << 4);
  }
  else {
    switch_status &= (~(1 << 4));
  }  

  if ((SW_K8_GPIO_Port->IDR&(SW_K8_Pin))>0) {
    switch_status |= ((!!(SW_K8_GPIO_Port->IDR&(SW_K8_Pin))) << 7);
  }
  else {
    switch_status &= (~(1 << 7));
  }  

  if ((SW_K6_GPIO_Port->IDR&(SW_K6_Pin))>0) {
    switch_status |= ((!!(SW_K6_GPIO_Port->IDR&(SW_K6_Pin))) << 5);
  }
  else {
    switch_status &= (~(1 << 5));
  }  

  if (is_irq_enable)
    GPIOA->BRR = GPIO_PIN_13;
  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim3_ch2);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles I2C2 global interrupt.
  */
__weak void I2C2_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_IRQn 0 */

  /* USER CODE END I2C2_IRQn 0 */

  /* USER CODE BEGIN I2C2_IRQn 1 */

  /* USER CODE END I2C2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
