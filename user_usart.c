/*
*********************************************************************************************************
*
*	ģ������ : �����շ�����ģ��Hal��汾
*	�ļ����� : user_usart.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
* �༭ʱ�� ��2025/4/7
*	��    �� ��LLX
* ��    ע ��ʹ���ж�+DMA
*            Ŀǰֻ�ܳ�ʼ��һ�����ڣ����Ľ�
*            Ŀǰ��Ҫ���USART1������չ�Դ���һ���Ľ�
*            �Բ�֧������
*********************************************************************************************************
*/

#include "user_usart.h"
#include <stdarg.h> // For va_list, va_start, va_end

// ���ջ����� (��Ϊ static�������������ڱ��ļ�)
static uint8_t USART1_RX_BUF[USART_REC_LEN];


/**
 * @brief  ����ָ�����ڵ� DMA �����жϽ���
 * @param  huart: UART handle
 * @param  pData: ָ����ջ�������ָ��
 * @param  Size:  ���ջ���������󳤶�
 * @retval None
 */
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart)
{
    // ȷ�������ָ��ʹ�С��Ч
    if (huart == NULL )
    {
        // ������Ӵ������߼��������ӡ������Ϣ�����
        return;
    }
    // ���� DMA ���գ�ʹ�ÿ����ж�ģʽ
    HAL_UARTEx_ReceiveToIdle_DMA(huart, USART1_RX_BUF, USART_REC_LEN);
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


/**
 * @brief  uart���ݽ��ջص�����������������жϺʹ�������ж�
 * @param  huart: UART handle
 * @param  Size:  ���յ������ݳ���
 * @retval None
 *         ����DMA�����ж�ʵ�ֲ��������ݽ���
 *         ��������ж�ͬ���ᴥ���ûص���������Ҫ�����ж�
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart->Instance == USART1)
  {
    if(huart->RxEventType == HAL_UART_RXEVENT_IDLE||huart->RxEventType == HAL_UART_RXEVENT_TC)//idle���У�tc������������ht�������
    {
      // --- �����ﴦ����յ������� ---
      // Size ������ʾ���ν��յ�����Ч���ݳ���
      // ���ݴ洢�� USART1_RX_BUF[0] �� USART1_RX_BUF[Size-1]

      // ʾ�����򵥻��Խ��յ�������
      // ע�⣺������ݴ���ʱ�������Ӧ����ʹ�ñ�־λ֪ͨ��ѭ������
      HAL_UART_Transmit_DMA(&huart1, USART1_RX_BUF, Size);//���ͽ��յ�������    
      // --- ������ջ����� (��ѡ��ȡ����Ӧ������) ---
      // ��������㣬�´ν��յ����ݻḲ�Ǿ����ݣ��� Size ��ָʾ�����ݵĳ���
      // memset(USART1_RX_BUF, 0, Size); // ֻ������յ��Ĳ���
      // memset(USART1_RX_BUF, 0, USART_REC_LEN); // ��������������

      // --- �������� DMA ���� ---
      // �����������������򲻻��ٽ�������
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, USART1_RX_BUF, USART_REC_LEN);//Ϊ�´ν��տ����жϣ�����ֻ��ִ��һ�� 
      //�������ӱ�־λ˵���˴�������ɣ����ݵ�������
    }    
  }
}

/*---------------------------------printf��������-----------------------------------*/
 
/* ʹ��microLib�ķ������ǵ�Ҫ��ħ����������й�ѡ */
//�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����

#ifdef __GNUC__            //gcc�������궨��
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/*�������˼�ǣ�
��������˺�__GNUC__����ʹ��GCC�������������#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
���û�ж����__GNUC__������ʹ��GCC�������������#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)  */
 
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);//��Ҫ��Ϊ��������ֱ���޸�USART1��2
    return ch;
}

/*---------------------------------��ѡ����װ printf-----------------------------------*/

/**
 * @brief  ��װһ�� printf ����������ֱ��ָ�����ڷ��� (�ڲ�ʹ�� DMA �� ��������)
 *         ע�⣺���ʵ��ʹ���˽ϴ��ջ�ռ䣬�����������ȴ���ʽ����ɡ�
 * @param  huart: UART handle
 * @param  format: ��ʽ���ַ���
 * @param  ...: �ɱ����
 * @retval None
 */
void User_USART_Printf(UART_HandleTypeDef *huart, const char *format, ...)
{
    if (huart == NULL || format == NULL) {
        return;
    }

    char buffer[256]; // ��ʱ�����������ڴ洢��ʽ������ַ�������С������Ҫ����
    va_list args;
    int len;

    va_start(args, format);
    // ʹ�� vsnprintf ��ֹ���������
    len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len > 0)
    {
        // ����ѡ��ʹ�� DMA ���ͻ���������
        // 1. DMA ���� (��������Ч�ʸߣ�����Ҫע�� buffer ����������)
        //    ��� buffer �Ǿֲ�������DMA �����ں������غ��ڴ��䣬�������ݴ���
        //    ��Ҫȷ�� DMA ������ɣ�����ʹ�þ�̬/ȫ�� buffer��
        //    Ϊ�����������ʹ���������͡�
        // User_USART_Transmit_DMA(huart, (uint8_t*)buffer, len);

        // 2. �������� (�򵥿ɿ�)
        HAL_UART_Transmit(huart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
    }
}

////��ʼ������
//void User_USART1_Init(UART_HandleTypeDef *huart1)
//{
//  HAL_UARTEx_ReceiveToIdle_DMA(huart1, USART1_RX_BUF, USART_REC_LEN);//�������������жϣ�DMA��
//}


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



//�����ض���fgetc������ʽ��fgetc��dma���ջ��ƴ��ڳ�ͻ��ֱ���ڻ�ص������д�����յ������ݼ���
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
