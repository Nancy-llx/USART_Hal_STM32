/*
*********************************************************************************************************
*
*	ģ������ : �����շ�����ģ�� (HAL��)
*	�ļ����� : user_usart.c
*	��    �� : V2.0
*	˵    �� : ʵ���ļ�
* �༭ʱ�� ��2025/4/7
*	��    �� ��(�������)
* ��    ע ��ʹ�� HAL �⣬�ж�+DMA���գ�printf�ض���
*            �Ƴ�����DMA���ճ�ͻ��fgetc�ض���
*            Ŀǰ��Ҫ���USART1������չ�Դ���һ���Ľ�
*********************************************************************************************************
*/

#include "user_usart.h"
#include <stdarg.h> // For va_list, va_start, va_end

// --- ȫ��/��̬���� ---

// ���ջ����� (��Ϊ static�������������ڱ��ļ�)
static uint8_t USART1_RX_BUF[USART_REC_LEN];

// printf �ض����Ŀ�괮�ھ�� (��Ϊ static)
static UART_HandleTypeDef *g_huart_printf = NULL;

// --- �������� ---

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
 * @brief  ʹ�� DMA ��������������
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
 * @brief  �������� printf �ض���Ĵ���
 * @param  huart: ָ��Ҫ���� printf ����� UART Handle
 * @retval None
 */
void User_USART_Set_Printf_Uart(UART_HandleTypeDef *huart)
{
    g_huart_printf = huart;
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_UARTEx_RxEventCallback
*	����˵��: UART DMA �����¼��ص����� (���� IDLE, TC, HT)
*	��    ��: huart: �����¼��� UART handle
*			  Size:  ���յ������ݴ�С (���� IDLE �¼��������ϴ� DMA �������������߿���ʱ���յ��ֽ���)
*	�� �� ֵ: None
* ��    ע������DMA�����ж�ʵ�ֲ��������ݽ���
*********************************************************************************************************
*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // --- ���� USART1 �Ľ����¼� ---
    if (huart->Instance == USART1) // ʹ�� Instance �Ƚϸ��ɿ�
    {
        // �ж��Ƿ��� IDLE �� TC (Transfer Complete) �¼�
        // TC ͨ�������ڻ�������ʱ��IDLE ���������߿���ʱ
        if (huart->RxEventType == HAL_UART_RXEVENT_IDLE || huart->RxEventType == HAL_UART_RXEVENT_TC)
        {
            // --- �����ﴦ����յ������� ---
            // Size ������ʾ���ν��յ�����Ч���ݳ���
            // ���ݴ洢�� USART1_RX_BUF[0] �� USART1_RX_BUF[Size-1]

            // ʾ�����򵥻��Խ��յ�������
            // ע�⣺������ݴ���ʱ�������Ӧ����ʹ�ñ�־λ֪ͨ��ѭ������
            User_USART_Transmit_DMA(&huart1, USART1_RX_BUF, Size); // ʹ�÷�װ�ĺ�������

            // --- ������ջ����� (��ѡ��ȡ����Ӧ������) ---
            // ��������㣬�´ν��յ����ݻḲ�Ǿ����ݣ��� Size ��ָʾ�����ݵĳ���
            // memset(USART1_RX_BUF, 0, Size); // ֻ������յ��Ĳ���
            // memset(USART1_RX_BUF, 0, USART_REC_LEN); // ��������������

            // --- �������� DMA ���� ---
            // �����������������򲻻��ٽ�������
            User_USART_Start_DMA_Receive(&huart1, USART1_RX_BUF, USART_REC_LEN); // ʹ�÷�װ�ĺ�������
        }
        // ����ѡ���Դ��� HT (Half Transfer) �¼��������ڴ�ʾ���д���
        // else if (huart->RxEventType == HAL_UART_RXEVENT_HT) { ... }
    }

    // --- �����������ڵĽ����¼� (�����Ҫ) ---
    /*
    else if (huart->Instance == USART2)
    {
        // ���� USART2 ���߼�...
        // ��ҪΪ USART2 ���建�����ʹ���ʽ
        // static uint8_t USART2_RX_BUF[USART_REC_LEN];
        // if (huart->RxEventType == HAL_UART_RXEVENT_IDLE || huart->RxEventType == HAL_UART_RXEVENT_TC)
        // {
        //     // ... ���� USART2 ���� ...
        //     User_USART_Start_DMA_Receive(&huart2, USART2_RX_BUF, USART_REC_LEN);
        // }
    }
    */
}

/*---------------------------------printf �����ض���-----------------------------------*/

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  �ض��� C �⺯�� printf ��ָ���Ĵ��� (����ʽ����)
 * @param  ch: Ҫ���͵��ַ�
 * @param  f: �ļ�ָ�� (δʹ��)
 * @retval ���ط��͵��ַ�
 */
PUTCHAR_PROTOTYPE
{
    if (g_huart_printf == NULL) {
        return -1; // ���û������Ŀ�괮�ڣ��򲻷���
    }

    // ʹ�� HAL �������ʽ���ͺ������͵����ַ�
    // HAL_MAX_DELAY ��ʾһֱ�ȴ�ֱ��������ɻ�ʱ (ʵ���� HAL ʵ����ͨ�������޵ȴ�)
    HAL_StatusTypeDef status = HAL_UART_Transmit(g_huart_printf, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        return ch;
    } else {
        return -1; // ����ʧ��
    }
}

/*---------------------------------fgetc / scanf �ض��� (���Ƴ�)-----------------------------------*/
/*
 * �����ض��� fgetc��
 * ԭ������ʽ�� fgetc ��������� DMA+IDLE ���ջ��ƴ��ڳ�ͻ��
 *      DMA �ں�̨�������ݿ飬�� fgetc �������ȴ������ַ��������߼����ҡ�
 * ����������� HAL_UARTEx_RxEventCallback �ص������д�����յ������� (USARTx_RX_BUF)��
 *          �������ñ�־λ������ѭ���������������е����ݡ�
 */
/*
// int fgetc(FILE *f)
// {
//     // �˺����� DMA IDLE ���ճ�ͻ�����Ƴ�
// }
*/


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