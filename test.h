/*
*********************************************************************************************************
*
*	模块名称 : 串口收发驱动模块 (HAL版)
*	文件名称 : user_usart.h
*	版    本 : V2.0
*	说    明 : 头文件
* 编辑时间 ：2025/4/7
*	作    者 ：(你的名字)
* 备    注 ：使用 HAL 库，中断+DMA接收，printf重定向
*            移除了与DMA接收冲突的fgetc重定向
*            目前主要针对USART1，可扩展性待进一步改进
*********************************************************************************************************
*/

#ifndef USER_USART_H
#define USER_USART_H

#include "stm32fxxx_hal.h" // 根据你的具体 STM32 型号包含对应的 HAL 头文件
#include <stdio.h>
#include <string.h>

// 定义接收缓冲区大小
#define USART_REC_LEN 256

// 外部声明需要使用的 UART Handle (例如在 main.c 中定义)
// 如果只用一个串口，可以这样声明；如果需要支持多个，则需要更灵活的设计
extern UART_HandleTypeDef huart1;

// 外部声明接收缓冲区 (如果需要在其他文件访问)
// 更好的做法是将其设为 user_usart.c 内的静态变量，通过函数接口访问
extern uint8_t USART1_RX_BUF[USART_REC_LEN];

// 函数原型声明
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void User_USART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
void User_USART_Printf(UART_HandleTypeDef *huart, const char *format, ...); // 可选的 printf 封装

// HAL 库回调函数声明 (虽然定义在 .c 文件，但有时放在头文件方便查看)
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size); // HAL库会自动调用，无需在此声明

// 设置 printf 重定向的目标串口 (需要在初始化时调用)
void User_USART_Set_Printf_Uart(UART_HandleTypeDef *huart);

#endif // USER_USART_H