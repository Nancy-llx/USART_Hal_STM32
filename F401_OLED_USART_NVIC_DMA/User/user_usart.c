/*
*********************************************************************************************************
*
*	ģ������ : �����շ�����ģ��
*	�ļ����� : user_usart.h
*	��    �� : V1.2
*	˵    �� : ͷ�ļ�
* �༭ʱ�� ��2025/4/7
*	��    �� ��
* ��    ע ��ʹ���ж�+DMA
*            Ŀǰֻ�ܳ�ʼ��һ�����ڣ����Ľ�
*********************************************************************************************************
*/

#include "user_usart.h"

#define USART_REC_LEN 256 // �������ֽ���
#define RXBUFFERSIZE 1 // �����С

unsigned char USART1_RX_BUF[USART_REC_LEN]; // ���ջ���
unsigned int USART1_RX_STA = 0; // ����״̬���
unsigned char aRxBuffer1[RXBUFFERSIZE]; // ���ջ���
unsigned char len1; // ���յ������ݳ���

////��ʼ������
//void User_USART1_Init(UART_HandleTypeDef *huart1)
//{
//  HAL_UARTEx_ReceiveToIdle_DMA(huart1, USART1_RX_BUF, USART_REC_LEN);//�������������жϣ�DMA��
//}
/**
 * @brief  ����ָ�����ڵ� DMA �����жϽ���
 * @param  huart: UART handle
 * @param  pData: ָ����ջ�������ָ��
 * @param  Size:  ���ջ���������󳤶�
 * @retval None
 */
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    // ȷ�������ָ��ʹ�С��Ч
    if (huart == NULL || pData == NULL || Size == 0)
    {
        // ������Ӵ������߼��������ӡ������Ϣ�����
        return;
    }
    // ���� DMA ���գ�ʹ�ÿ����ж�ģʽ
    HAL_UARTEx_ReceiveToIdle_DMA(huart, pData, Size);
}

/**
 * @brief  ʹ�� DMA �������������ݣ����飩
 * @param  huart: UART handle
 * @param  pData: ָ��Ҫ�������ݵ�ָ��
 * @param  Size:  Ҫ���͵����ݳ���
 * @retval None
 */
void User_USART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
    if (huart == NULL || pData == NULL || Size == 0)
    {
        return; // �����Ĳ������
    }
    HAL_UART_Transmit_DMA(huart, pData, Size);
    // ע�⣺DMA �������첽�ġ������Ҫȷ��������ɣ�
    // ��Ҫ��� HAL_UART_GetState() ��ʹ�÷�����ɻص� HAL_UART_TxCpltCallback
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_UARTEx_RxEventCallback
*	����˵��: uart���ݽ��ջص�����������������жϺʹ�������ж�
*	��    ��: _*huart: ��Ӧ����
*			      Size: ���յ������ݴ�С
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
* ��    ע������DMA�����ж�ʵ�ֲ��������ݽ���
*           ��������ж�ͬ���ᴥ���ûص���������Ҫ�����ж�
*********************************************************************************************************
*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart->Instance == USART1)
  {
    if(huart->RxEventType == HAL_UART_RXEVENT_IDLE||huart->RxEventType == HAL_UART_RXEVENT_TC)//idle���У�tc������������ht�������
    {
      HAL_UART_Transmit_DMA(&huart1, USART1_RX_BUF, Size);//���ͽ��յ�������    
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, USART1_RX_BUF, USART_REC_LEN);//Ϊ�´ν��տ����жϣ�����ֻ��ִ��һ�� 
      //�������ӱ�־λ˵���˴�������ɣ����ݵ�������
    }    
  }
}

////������ʽ��������
//void USART1_Printf(const char* str)
//{
//    unsigned int usart1_len = strlen(str);
////    unsigned int index = 0;
//    HAL_UART_Transmit_DMA(&huart1, (const unsigned char *)str, strlen(str));//���ͽ��յ�������    
//    if (str == NULL) {
//        return;
//    }
//    
////    while (index < usart1_len) {
////        HAL_UART_Transmit(&huart1, (unsigned char*)&str[index], 1, 0xff);
////        index++;
////    }
//}

/*---------------------------------printf��������-----------------------------------*/
 
/* ʹ��microLib�ķ������ǵ�Ҫ��ħ����������й�ѡ */
//�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);//��Ҫ��Ϊ��������ֱ���޸�USART1��2
    return ch;
}

//�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���

//int fgetc(FILE *f)
//{
//    /* �ȴ������������� */
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
// 
//    return (int)USART_ReceiveData(USART1);
//}




//void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
//{
//    if (huart->Instance == USART1) {
//        if ((USART1_RX_STA & 0x8000) == 0) // ����δ���
//        {
//            if (USART1_RX_STA & 0x4000) // ���յ���0x0d
//            {
//                if (aRxBuffer1[0] != 0x0a) // ���մ���,���¿�ʼ
//                    USART1_RX_STA = 0;
//                else // ���������
//                    USART1_RX_STA |= 0x8000;
//            } else // ��û�յ�0X0D
//            {
//                if (aRxBuffer1[0] == 0x0d)
//                    USART1_RX_STA |= 0x4000;
//                else {
//                    USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = aRxBuffer1[0];
//                    USART1_RX_STA++;
//                    if (USART1_RX_STA > (USART_REC_LEN - 1))
//                        USART1_RX_STA = 0; // �������ݴ���,���¿�ʼ����
//                }
//            }
//        }
//    }
//}
//void USART1_IRQHandler(void)
//{
//    unsigned int timeout = 0;
//    unsigned int maxDelay = 0x1FFFF;

//    HAL_UART_IRQHandler(&huart1); // ����HAL���жϴ����ú���

//    timeout = 0;
//    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) // �ȴ�����
//    {
//        timeout++; // ��ʱ����
//        if (timeout > maxDelay)
//            break;
//    }

//    timeout = 0;
//    // һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
//    while (HAL_UART_Receive_IT(&huart1, (unsigned char*)aRxBuffer1, RXBUFFERSIZE) != HAL_OK) {
//        timeout++; // ��ʱ����
//        if (timeout > maxDelay)
//            break;
//    }
//}
