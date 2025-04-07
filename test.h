/*
*********************************************************************************************************
*
*	ģ������ : �����շ�����ģ�� (HAL��)
*	�ļ����� : user_usart.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
* �༭ʱ�� ��2025/4/7
*	��    �� ��(�������)
* ��    ע ��ʹ�� HAL �⣬�ж�+DMA���գ�printf�ض���
*            �Ƴ�����DMA���ճ�ͻ��fgetc�ض���
*            Ŀǰ��Ҫ���USART1������չ�Դ���һ���Ľ�
*********************************************************************************************************
*/

#ifndef USER_USART_H
#define USER_USART_H

#include "stm32fxxx_hal.h" // ������ľ��� STM32 �ͺŰ�����Ӧ�� HAL ͷ�ļ�
#include <stdio.h>
#include <string.h>

// ������ջ�������С
#define USART_REC_LEN 256

// �ⲿ������Ҫʹ�õ� UART Handle (������ main.c �ж���)
// ���ֻ��һ�����ڣ��������������������Ҫ֧�ֶ��������Ҫ���������
extern UART_HandleTypeDef huart1;

// �ⲿ�������ջ����� (�����Ҫ�������ļ�����)
// ���õ������ǽ�����Ϊ user_usart.c �ڵľ�̬������ͨ�������ӿڷ���
extern uint8_t USART1_RX_BUF[USART_REC_LEN];

// ����ԭ������
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void User_USART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
void User_USART_Printf(UART_HandleTypeDef *huart, const char *format, ...); // ��ѡ�� printf ��װ

// HAL ��ص��������� (��Ȼ������ .c �ļ�������ʱ����ͷ�ļ�����鿴)
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size); // HAL����Զ����ã������ڴ�����

// ���� printf �ض����Ŀ�괮�� (��Ҫ�ڳ�ʼ��ʱ����)
void User_USART_Set_Printf_Uart(UART_HandleTypeDef *huart);

#endif // USER_USART_H