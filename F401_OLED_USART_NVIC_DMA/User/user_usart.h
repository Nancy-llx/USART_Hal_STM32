/*
*********************************************************************************************************
*
*	模块名称 : 串口收发驱动模块Hal库版本头文件
*	文件名称 : user_usart.h
*	版    本 : V2.0
*	说    明 : 头文件
* 编辑时间 ：2025/4/7
*	作    者 ：
*
*********************************************************************************************************
*/

#ifndef USART_USART_H_
#define USART_USART_H_

#include "stm32f4xx_hal.h"
//#include "usart.h"
#include "string.h"
#include "stdio.h"

// 定义接收缓冲区大小
#define USART_REC_LEN 256 // 最大接收字节数

// 外部声明需要使用的 UART Handle 
extern UART_HandleTypeDef huart1;

void User_USART1_Init(UART_HandleTypeDef *huart1);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart);
void USART1_Printf(const char* str);

void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void User_USART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);// HAL库会自动调用，无需在此声明
void User_USART_Printf(UART_HandleTypeDef *huart, const char *format, ...);//可选函数

#endif 



/* 定义使能的串口, 0 表示不使能（不增加代码大小）， 1表示使能 */
 

//#define	UART1_FIFO_EN	1    ///* USART1  PA9, PA10 或  PB6, PB7*/
//#define	UART2_FIFO_EN	0
//#define	UART3_FIFO_EN	0
//#define	UART4_FIFO_EN	0
//#define	UART5_FIFO_EN	0
//#define	UART6_FIFO_EN	0


///* 定义端口号 */
//typedef enum
//{
//	COM1 = 0,	/* USART1  PA9, PA10 或  PB6, PB7*/
//	COM2 = 1,	/* USART2, PD5,PD6 或 PA2, PA3 */
//	COM3 = 2,	/* USART3, PB10, PB11 */
//	COM4 = 3,	/* UART4, PC10, PC11 */
//	COM5 = 4,	/* UART5, PC12, PD2 */
//	COM6 = 5	/* USART6, PC6, PC7 */
//}COM_PORT_E;

