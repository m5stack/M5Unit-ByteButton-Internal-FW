/**
  ******************************************************************************
  * @file    ws2812.c
  * @brief   This file provides code for the configuration
  *          of the PWM instances.
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
#include "ws2812.h"
#include <stdlib.h>

/* USER CODE BEGIN 0 */
#include <string.h>
#include "tim.h"
/* USER CODE END 0 */

uint32_t *color_buf = NULL;
uint8_t rled[PIXEL_MAX] = {0};
uint8_t gled[PIXEL_MAX] = {0};
uint8_t bled[PIXEL_MAX] = {0};
uint8_t brightness_index[PIXEL_MAX] = {0};
uint32_t *lastest_rgb_color = NULL;
uint8_t *lastest_rgb_233_color = NULL;
FIFOBuffer* rgb_buffer;
FIFOBuffer_RGB233* rgb_233_buffer;
uint8_t rgb_show_lock = 0;

frame_buf frame = { 
	.head[0] = 0,
	.head[1] = 0,
	.head[2] = 0,
	.tail = 0,
};

FIFOBuffer* createBuffer() {
    FIFOBuffer* buffer = (FIFOBuffer*)malloc(sizeof(FIFOBuffer));
    buffer->front = buffer->rear = NULL;
    return buffer;
}

void enqueue(FIFOBuffer* buffer, uint32_t newData[PIXEL_MAX]) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    memcpy(newNode->data, newData, PIXEL_MAX * sizeof(uint32_t));
    newNode->next = NULL;
    
    if (buffer->rear == NULL) {
        buffer->front = buffer->rear = newNode;
        return;
    }
    
    buffer->rear->next = newNode;
    buffer->rear = newNode;
}

int8_t dequeue(FIFOBuffer* buffer, uint32_t result[PIXEL_MAX]) {
    if (buffer->front == NULL) {
        return 0;
    }

    Node* temp = buffer->front;
    memcpy(result, temp->data, PIXEL_MAX * sizeof(uint32_t));
    buffer->front = buffer->front->next;

    if (buffer->front == NULL) {
        buffer->rear = NULL;
    }

    free(temp);

    return 1;
}

uint32_t* getQueueRear(FIFOBuffer* buffer) {
    if (buffer->rear != NULL) {
        return buffer->rear->data;
    } else {
        return NULL; // 如果队列为空，返回 NULL
    }
}

FIFOBuffer_RGB233* createBuffer_rgb233() {
    FIFOBuffer_RGB233* buffer = (FIFOBuffer_RGB233*)malloc(sizeof(FIFOBuffer_RGB233));
    buffer->front = buffer->rear = NULL;
    return buffer;
}

void enqueue_rgb233(FIFOBuffer_RGB233* buffer, uint8_t newData[PIXEL_MAX]) {
    Node_RGB233* newNode = (Node_RGB233*)malloc(sizeof(Node_RGB233));
    memcpy(newNode->data, newData, PIXEL_MAX * sizeof(uint8_t));
    newNode->next = NULL;
    
    if (buffer->rear == NULL) {
        buffer->front = buffer->rear = newNode;
        return;
    }
    
    buffer->rear->next = newNode;
    buffer->rear = newNode;
}

int8_t dequeue_rgb233(FIFOBuffer_RGB233* buffer, uint8_t result[PIXEL_MAX]) {
    if (buffer->front == NULL) {
        return 0;
    }

    Node_RGB233* temp = buffer->front;
    memcpy(result, temp->data, PIXEL_MAX * sizeof(uint8_t));
    buffer->front = buffer->front->next;

    if (buffer->front == NULL) {
        buffer->rear = NULL;
    }

    free(temp);

    return 1;
}

uint8_t* getQueueRear_rgb233(FIFOBuffer_RGB233* buffer) {
    if (buffer->rear != NULL) {
        return buffer->rear->data;
    } else {
        return NULL; // 如果队列为空，返回 NULL
    }
}

void sk6812_init(uint8_t num) {
  color_buf = (uint32_t *)calloc(num, sizeof(uint32_t));
  lastest_rgb_color = (uint32_t *)calloc(num, sizeof(uint32_t));
  lastest_rgb_233_color = (uint8_t *)calloc(num, sizeof(uint8_t));
  rgb_buffer = createBuffer();
  rgb_233_buffer = createBuffer_rgb233();
  for (int i = 0; i < PIXEL_MAX; i++) {
    brightness_index[i] = 255;
  }
}

void neopixel_set_color(uint8_t num, uint32_t color) {
  uint16_t r, g, b;
  if (num >= PIXEL_MAX)
    return;
  
  if (brightness_index[num] < 10 && brightness_index[num])
    brightness_index[num] = 10;

  r = ((color >> 16) & 0xff) * ((float)brightness_index[num] / 255.0f);
  g = ((color >> 8) & 0xff) * ((float)brightness_index[num] / 255.0f);
  b = (color & 0xff) * ((float)brightness_index[num] / 255.0f);

  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
	rled[num] = r;
	gled[num] = g;
	bled[num] = b;
	color_buf[num] = color;
}

uint32_t rgb233_to_rgb888(uint8_t num, uint8_t color)
{
  uint32_t rgb_888 = 0;
  uint8_t r_led, g_led, b_led;

  if (num >= PIXEL_MAX)
    return 0xFFFFFF;  

  r_led = ((color & 0xC0) >> 6);
  g_led = ((color & 0x38) >> 3);
  b_led = (color & 0x07);

  b_led = ((b_led << 5) | ((b_led & 0x03) << 3) | b_led);
  g_led = ((g_led << 5) | ((g_led & 0x03) << 3) | g_led);
  r_led |= (r_led << 2);
  r_led |= (r_led << 4);

  rgb_888 = ((r_led << 16) | (g_led << 8) | b_led);

  neopixel_set_color(num, rgb_888);
  return rgb_888;
}

uint8_t rgb888_to_rgb233(uint32_t color)
{
  uint8_t r_led, g_led, b_led, rgb_233;

  r_led = ((color >> 16) & 0xff);
  g_led = ((color >> 8) & 0xff);
  b_led = (color & 0xff);

  rgb_233 = ((r_led & 0xC0) | ((g_led & 0xE0) >> 2) | ((b_led & 0xE0) >> 5));

  return rgb_233;
}

void ws2812_show(void)
{
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_2);
	uint8_t i, j;

	for(i = 0; i < PIXEL_MAX; i++) {
		for(j = 0; j < 8; j++) {														// G->R->B
			 frame.data[24 * i + j] = (gled[i] & (0x80 >> j)) ? BIT_1 : BIT_0; 			// 将高低位扩展到16bit
			 frame.data[24 * i + j + 8]   = (rled[i] & (0x80 >> j)) ? BIT_1 : BIT_0;	
			 frame.data[24 * i + j + 16]  = (bled[i] & (0x80 >> j)) ? BIT_1 : BIT_0;
		}
	}

	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)&frame, 3 + 24 * PIXEL_MAX + 1);
}
/* USER CODE BEGIN 2 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
  rgb_show_lock = 0;
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
