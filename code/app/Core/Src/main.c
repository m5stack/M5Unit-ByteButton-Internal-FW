/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "ws2812.h"
#include "flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define I2C_ADDRESS 0x47
#define FIRMWARE_VERSION 1
#define APPLICATION_ADDRESS     ((uint32_t)0x08001800) 
#define FLASH_DATA_SIZE 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// i2c address
uint8_t i2c_address[1] = {0};
// flash data
static uint8_t flash_data[FLASH_DATA_SIZE] = {0};

// jump bootloader
volatile static uint8_t flag_jump_bootloader = 0;
volatile static uint32_t jump_bootloader_timeout = 0;

// firmware version
volatile static uint8_t fm_version = FIRMWARE_VERSION;

// i2c timeout handle
volatile static uint32_t i2c_stop_timeout_delay = 0;

// switch status
volatile uint8_t switch_status = 0;
static uint8_t switch_status_set[8] = {0};

// irq handle
volatile uint8_t is_irq_enable = 0;

// rgb show mode
static uint8_t rgb_show_mode = 0;
static uint32_t sys_rgb_color_switch_0[8] = {0};
static uint32_t sys_rgb_color_switch_1[8] = {0};

// flash write back flag
static uint8_t is_flash_write_back = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void IAP_Set()
{
	uint8_t i;
 
	uint32_t *pVecTab=(uint32_t *)(0x20000000);

	for(i = 0; i < 48; i++)
	{
		*(pVecTab++) = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
	}
  /* Enable the SYSCFG peripheral clock*/
#if 1 //STM32
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  __HAL_SYSCFG_REMAPMEMORY_SRAM();
#else //AMP32
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
    /* Remap SRAM at 0x00000000 */
    SYSCFG->CFG1_B.MMSEL = SYSCFG_MemoryRemap_SRAM;
#endif
}

void i2c_port_set_to_input(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void irq_port_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void init_flash_data(void) 
{   
  if (!(readPackedMessageFromFlash(flash_data, FLASH_DATA_SIZE))) {
    i2c_address[0] = I2C_ADDRESS;

    flash_data[0] = i2c_address[0];
    flash_data[1] = is_irq_enable;
    memcpy(&flash_data[2], (uint8_t *)sys_rgb_color_switch_0, 32);
    memcpy(&flash_data[2+32], (uint8_t *)sys_rgb_color_switch_1, 32);
    flash_data[2+32+32] = rgb_show_mode;
    writeMessageToFlash(flash_data , FLASH_DATA_SIZE);
  } else {
    i2c_address[0] = flash_data[0];
    is_irq_enable = flash_data[1];
    memcpy((uint8_t *)sys_rgb_color_switch_0, &flash_data[2], 32);
    memcpy((uint8_t *)sys_rgb_color_switch_1, &flash_data[2+32], 32);
    rgb_show_mode = flash_data[2+32+32];
  }
}

void flash_data_write_back(void)
{
  if (readPackedMessageFromFlash(flash_data, FLASH_DATA_SIZE)) {
    flash_data[0] = i2c_address[0];
    flash_data[1] = is_irq_enable;
    memcpy(&flash_data[2], (uint8_t *)sys_rgb_color_switch_0, 32);
    memcpy(&flash_data[2+32], (uint8_t *)sys_rgb_color_switch_1, 32);
    flash_data[2+32+32] = rgb_show_mode;
    writeMessageToFlash(flash_data , FLASH_DATA_SIZE);
  }     
}

void init_swtich_status(void)
{
  switch_status |= (!!(SW_K1_GPIO_Port->IDR&(SW_K1_Pin)));
  switch_status |= ((!!(SW_K2_GPIO_Port->IDR&(SW_K2_Pin))) << 1);
  switch_status |= ((!!(SW_K3_GPIO_Port->IDR&(SW_K3_Pin))) << 2);
  switch_status |= ((!!(SW_K4_GPIO_Port->IDR&(SW_K4_Pin))) << 3);
  switch_status |= ((!!(SW_K5_GPIO_Port->IDR&(SW_K5_Pin))) << 4);
  switch_status |= ((!!(SW_K6_GPIO_Port->IDR&(SW_K6_Pin))) << 5);
  switch_status |= ((!!(SW_K7_GPIO_Port->IDR&(SW_K7_Pin))) << 6);
  switch_status |= ((!!(SW_K8_GPIO_Port->IDR&(SW_K8_Pin))) << 7);
}

void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
  uint8_t buf[48];
  uint8_t rx_buf[48];
  uint8_t rx_mark[48] = {0}; 

  if (len > 1) {
    if (rx_data[0] == 0xFF) 
    {
      if (len == 2) {
        if (rx_data[1] < 128) {
          i2c_address[0] = rx_data[1];
          is_flash_write_back = 1;
          user_i2c_init();
        }
      }
    } 
    else if (rx_data[0] == 0xF1) 
    {
      is_irq_enable = !!rx_data[1];
      is_flash_write_back = 1;
      if (is_irq_enable) {
        irq_port_init();
      }
    } 
    else if (rx_data[0] == 0xF0) 
    {
      is_flash_write_back = 1;
    } 
    else if ((rx_data[0] >= 0x10) && (rx_data[0] <= 0x19)) {
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]-0x10+i;
        
        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
        if (rx_mark[9] == 0 && rx_mark[10] == 0)     
          brightness_index[rx_index] = rx_buf[rx_index];
        else if (rx_mark[9] && rx_mark[10] == 0 && rx_index == 9)
          rgb_show_mode = rx_buf[rx_index];
      }     
    }    
    else if ((rx_data[0] >= 0x20) && (rx_data[0] <= 0x43)) {
      uint32_t *rgb_point_temp = getQueueRear(rgb_buffer);
      if (rgb_point_temp != NULL) {
        memcpy((uint8_t *)lastest_rgb_color, (uint8_t *)rgb_point_temp, PIXEL_MAX*4);
        enqueue(rgb_buffer, lastest_rgb_color);
      }
      else {
        uint32_t rgb_temp[PIXEL_MAX] = {0};
        memcpy(rgb_temp, lastest_rgb_color, sizeof(rgb_temp));
        enqueue(rgb_buffer, rgb_temp);
      }  
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]-0x20+i;

        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
        if (rx_mark[36] == 0) {
          ((uint8_t *)lastest_rgb_color)[rx_index] = rx_buf[rx_index];
          ((uint8_t *)rgb_buffer->rear->data)[rx_index] = rx_buf[rx_index];
        }
      }                
    } 
    else if ((rx_data[0] >= 0x70) && (rx_data[0] <= 0x8F)) {
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]-0x70+i;

        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
        ((uint8_t *)sys_rgb_color_switch_0)[rx_index] = rx_buf[rx_index];
      }                
    } 
    else if ((rx_data[0] >= 0x90) && (rx_data[0] <= 0xAF)) {
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]-0x90+i;

        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
        ((uint8_t *)sys_rgb_color_switch_1)[rx_index] = rx_buf[rx_index];
      }                
    } 
    else if ((rx_data[0] >= 0x50) && (rx_data[0] <= 0x58)) {
      uint8_t *rgb_233_temp = getQueueRear_rgb233(rgb_233_buffer);
      if (rgb_233_temp != NULL) {
        memcpy((uint8_t *)lastest_rgb_233_color, (uint8_t *)rgb_233_temp, PIXEL_MAX);
        enqueue_rgb233(rgb_233_buffer, lastest_rgb_233_color);
      }
      else {
        uint8_t rgb_temp[PIXEL_MAX] = {0};
        memcpy(rgb_temp, lastest_rgb_233_color, PIXEL_MAX);
        enqueue_rgb233(rgb_233_buffer, rgb_temp);
      }  
      for(int i = 0; i < len - 1; i++) {
        uint8_t rx_index = rx_data[0]-0x50+i;

        rx_buf[rx_index] = rx_data[1+i];
        rx_mark[rx_index] = 1;
        if (rx_mark[9] == 0) {
          ((uint8_t *)lastest_rgb_233_color)[rx_index] = rx_buf[rx_index];
          ((uint8_t *)rgb_233_buffer->rear->data)[rx_index] = rx_buf[rx_index];
        }
      }                
    } 
    else if (rx_data[0] == 0xFD)
    {
      if (rx_data[1] == 1) {
        flag_jump_bootloader = 1;
        if (flag_jump_bootloader) {
          LL_I2C_DeInit(I2C2);
          LL_I2C_DisableAutoEndMode(I2C2);
          LL_I2C_Disable(I2C2);
          LL_I2C_DisableIT_ADDR(I2C2);
          HAL_TIM_PWM_MspDeInit(&htim3);
          my_gpio_deinit();
          i2c_port_set_to_input();
          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) || HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12))
          {
            jump_bootloader_timeout++;
            if (jump_bootloader_timeout >= 60000) {
              flag_jump_bootloader = 0;
              break;
            }
          }
          if (jump_bootloader_timeout < 60000) {
            NVIC_SystemReset();
          } else {
            MX_GPIO_Init();
            MX_DMA_Init();
            MX_TIM3_Init(); 
            user_i2c_init(); 
            i2c2_it_enable(); 
            jump_bootloader_timeout = 0;
          }
        }        
      }   
    }      
  }
  else if (len == 1) {
    if (rx_data[0] == 0xFF) 
    {
      i2c2_set_send_data(i2c_address, 1);
    }    
    else if (rx_data[0] == 0xFE) 
    {
      i2c2_set_send_data((uint8_t *)&fm_version, 1);
    }
    else if (rx_data[0] == 0xF1) 
    {
      i2c2_set_send_data((uint8_t *)&is_irq_enable, 1);
    }
    else if (rx_data[0] == 0) {
      if (is_irq_enable)
        GPIOA->BSRR = GPIO_PIN_13;
      i2c2_set_send_data((uint8_t *)&switch_status, 1);
    }
    else if ((rx_data[0] >= 0x60) && (rx_data[0] <= 0x67)) {
      for (int i = 0; i < 8; i++) {
        switch_status_set[i] = (!!(switch_status & (1 << i)));
        buf[i] = switch_status_set[i];
      }
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x60], 0x67-rx_data[0]+1);
    }
    else if ((rx_data[0] >= 0x10) && (rx_data[0] <= 0x19)) {
      memcpy(buf, brightness_index, 9);
      buf[9] = rgb_show_mode;
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x10], 0x19-rx_data[0]+1);
    }     
    else if ((rx_data[0] >= 0x20) && (rx_data[0] <= 0x43)) {
      memcpy(buf, (uint8_t *)lastest_rgb_color, PIXEL_MAX*4);
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x20], 0x43-rx_data[0]+1);               
    }
    else if ((rx_data[0] >= 0x50) && (rx_data[0] <= 0x58)) {
      memcpy(buf, (uint8_t *)lastest_rgb_233_color, PIXEL_MAX);
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x50], 0x58-rx_data[0]+1);               
    }
    else if ((rx_data[0] >= 0x70) && (rx_data[0] <= 0x8F)) {
      memcpy(buf, (uint8_t *)sys_rgb_color_switch_0, 8*4);
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x70], 0x8F-rx_data[0]+1);                
    } 
    else if ((rx_data[0] >= 0x90) && (rx_data[0] <= 0xAF)) {
      memcpy(buf, (uint8_t *)sys_rgb_color_switch_1, 8*4);
      i2c2_set_send_data((uint8_t *)&buf[rx_data[0]-0x90], 0xAF-rx_data[0]+1);               
    }     
  }
 
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  IAP_Set();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  // MX_I2C2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  init_flash_data();
  if (is_irq_enable) {
    irq_port_init();
  }
  sk6812_init(PIXEL_MAX);
  init_swtich_status();
  fm_version = FIRMWARE_VERSION;

  user_i2c_init(); 
  i2c2_it_enable(); 
  for (int i = 0; i < PIXEL_MAX; i++) {
    neopixel_set_color(i, 0);
  }
  ws2812_show();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    i2c_timeout_counter = 0;
    if (i2c_stop_timeout_flag) {
      if (i2c_stop_timeout_delay < HAL_GetTick()) {
        i2c_stop_timeout_counter++;
        i2c_stop_timeout_delay = HAL_GetTick() + 10;
      }
    }
    if (i2c_stop_timeout_counter > 50) {
      LL_I2C_DeInit(I2C2);
      LL_I2C_DisableAutoEndMode(I2C2);
      LL_I2C_Disable(I2C2);
      LL_I2C_DisableIT_ADDR(I2C2);     
      user_i2c_init();    
      i2c2_it_enable();
      HAL_Delay(500);
    } 

    uint32_t rgb_show_temp[PIXEL_MAX] = {0};
    uint8_t rgb_233_show_temp[PIXEL_MAX] = {0};
    if (!rgb_show_lock) {
      if (rgb_show_mode) {
          for (int i = 0; i < 8; i++) {
            if ((switch_status >> i) & 0x01) {
              neopixel_set_color(i, sys_rgb_color_switch_1[i]);
            }
            else {
              neopixel_set_color(i, sys_rgb_color_switch_0[i]);
            }
          }
          neopixel_set_color(8, lastest_rgb_color[8]);
          rgb_show_lock = 1;
          ws2812_show();        
      }
      if (dequeue(rgb_buffer, rgb_show_temp) == 1) {
        if (!rgb_show_mode) {
          for (int i = 0; i < PIXEL_MAX; i++) {
            neopixel_set_color(i, rgb_show_temp[i]);
          }
          rgb_show_lock = 1;
          ws2812_show();
        }
      }
      if (dequeue_rgb233(rgb_233_buffer, rgb_233_show_temp) == 1) {
        if (!rgb_show_mode) {
          for (int i = 0; i < PIXEL_MAX; i++) {
            rgb233_to_rgb888(i, rgb_233_show_temp[i]);
          }
          rgb_show_lock = 1;
          ws2812_show();
        }
      }
    }
    if (is_flash_write_back) {
      flash_data_write_back();
      is_flash_write_back = 0;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    NVIC_SystemReset();
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
