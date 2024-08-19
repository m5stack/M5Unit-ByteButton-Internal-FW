/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ws2812.h
  * @brief   This file contains all the function prototypes for
  *          the ws2812.c file
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
#ifndef __ws2812_H__
#define __ws2812_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define PIXEL_MAX 9
#define RGB_BUFFER_SIZE 10
#define BIT_1 59
#define BIT_0 20

typedef struct Node {
    uint32_t data[PIXEL_MAX];
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} FIFOBuffer;

typedef struct Node_RGB233 {
    uint8_t data[PIXEL_MAX];
    struct Node_RGB233* next;
} Node_RGB233;

typedef struct {
    Node_RGB233* front;
    Node_RGB233* rear;
} FIFOBuffer_RGB233;

/* USER CODE END Private defines */
extern uint8_t brightness_index[PIXEL_MAX];
extern uint32_t *color_buf;
extern uint32_t *lastest_rgb_color;
extern uint8_t *lastest_rgb_233_color;
extern FIFOBuffer* rgb_buffer;
extern FIFOBuffer_RGB233* rgb_233_buffer;
extern uint8_t rgb_show_lock;

void sk6812_init(uint8_t num);
void neopixel_set_color(uint8_t num, uint32_t color);
void ws2812_show(void);
uint8_t rgb888_to_rgb233(uint32_t color);
uint32_t rgb233_to_rgb888(uint8_t num, uint8_t color);
FIFOBuffer* createBuffer();
void enqueue(FIFOBuffer* buffer, uint32_t newData[PIXEL_MAX]);
int8_t dequeue(FIFOBuffer* buffer, uint32_t result[PIXEL_MAX]);
uint32_t* getQueueRear(FIFOBuffer* buffer);
FIFOBuffer_RGB233* createBuffer_rgb233();
void enqueue_rgb233(FIFOBuffer_RGB233* buffer, uint8_t newData[PIXEL_MAX]);
int8_t dequeue_rgb233(FIFOBuffer_RGB233* buffer, uint8_t result[PIXEL_MAX]);
uint8_t* getQueueRear_rgb233(FIFOBuffer_RGB233* buffer);

/* USER CODE BEGIN Prototypes */
typedef struct
{
  const uint16_t head[3];           //先发送3个0等待DMA稳定
  uint16_t data[24 * PIXEL_MAX];    //真正的数据
  const uint16_t tail;              //发送最后1个0，保证DMA结束后，PWM输出低
} frame_buf;

/* USER CODE END Prototypes */

#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
