/*
*********************************************************************************************************
*
*	模块名称 : 串口收发驱动模块
*	文件名称 : user_usart.h
*	版    本 : V1.2
*	说    明 : 头文件
* 编辑时间 ：2025/4/7
*	作    者 ：
* 备    注 ：使用中断+DMA
*            目前只能初始化一个串口，待改进
*********************************************************************************************************
*/

#include "user_usart.h"

#define USART_REC_LEN 256 // 最大接收字节数
#define RXBUFFERSIZE 1 // 缓存大小

unsigned char USART1_RX_BUF[USART_REC_LEN]; // 接收缓存
unsigned int USART1_RX_STA = 0; // 接收状态标记
unsigned char aRxBuffer1[RXBUFFERSIZE]; // 接收缓冲
unsigned char len1; // 接收到的数据长度

////初始化程序
//void User_USART1_Init(UART_HandleTypeDef *huart1)
//{
//  HAL_UARTEx_ReceiveToIdle_DMA(huart1, USART1_RX_BUF, USART_REC_LEN);//开启接收数据中断（DMA）
//}
/**
 * @brief  启动指定串口的 DMA 空闲中断接收
 * @param  huart: UART handle
 * @param  pData: 指向接收缓冲区的指针
 * @param  Size:  接收缓冲区的最大长度
 * @retval None
 */
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    // 确保传入的指针和大小有效
    if (huart == NULL || pData == NULL || Size == 0)
    {
        // 可以添加错误处理逻辑，例如打印错误信息或断言
        return;
    }
    // 启动 DMA 接收，使用空闲中断模式
    HAL_UARTEx_ReceiveToIdle_DMA(huart, pData, Size);
}

/**
 * @brief  使用 DMA 非阻塞发送数据（数组）
 * @param  huart: UART handle
 * @param  pData: 指向要发送数据的指针
 * @param  Size:  要发送的数据长度
 * @retval None
 */
void User_USART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
    if (huart == NULL || pData == NULL || Size == 0)
    {
        return; // 基本的参数检查
    }
    HAL_UART_Transmit_DMA(huart, pData, Size);
    // 注意：DMA 发送是异步的。如果需要确保发送完成，
    // 需要检查 HAL_UART_GetState() 或使用发送完成回调 HAL_UART_TxCpltCallback
}

/*
*********************************************************************************************************
*	函 数 名: HAL_UARTEx_RxEventCallback
*	功能说明: uart数据接收回调函数，处理传输完成中断和传输过半中断
*	形    参: _*huart: 对应串口
*			      Size: 接收到的数据大小
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
* 备    注：利用DMA空闲中断实现不定长数据接收
*           传输过半中断同样会触发该回调函数，需要进行判断
*********************************************************************************************************
*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart->Instance == USART1)
  {
    if(huart->RxEventType == HAL_UART_RXEVENT_IDLE||huart->RxEventType == HAL_UART_RXEVENT_TC)//idle空闲，tc超出数据量，ht传输过半
    {
      HAL_UART_Transmit_DMA(&huart1, USART1_RX_BUF, Size);//发送接收到的数据    
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, USART1_RX_BUF, USART_REC_LEN);//为下次接收开启中断，否则只会执行一次 
      //可以增加标志位说明此处传输完成，传递到主函数
    }    
  }
}

////无阻塞式发送数据
//void USART1_Printf(const char* str)
//{
//    unsigned int usart1_len = strlen(str);
////    unsigned int index = 0;
//    HAL_UART_Transmit_DMA(&huart1, (const unsigned char *)str, strlen(str));//发送接收到的数据    
//    if (str == NULL) {
//        return;
//    }
//    
////    while (index < usart1_len) {
////        HAL_UART_Transmit(&huart1, (unsigned char*)&str[index], 1, 0xff);
////        index++;
////    }
//}

/*---------------------------------printf函数配置-----------------------------------*/
 
/* 使用microLib的方法，记得要在魔术棒哪里进行勾选 */
//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);//若要改为其他串口直接修改USART1、2
    return ch;
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数

//int fgetc(FILE *f)
//{
//    /* 等待串口输入数据 */
//    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
// 
//    return (int)USART_ReceiveData(USART1);
//}




//void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
//{
//    if (huart->Instance == USART1) {
//        if ((USART1_RX_STA & 0x8000) == 0) // 接收未完成
//        {
//            if (USART1_RX_STA & 0x4000) // 接收到了0x0d
//            {
//                if (aRxBuffer1[0] != 0x0a) // 接收错误,重新开始
//                    USART1_RX_STA = 0;
//                else // 接收完成了
//                    USART1_RX_STA |= 0x8000;
//            } else // 还没收到0X0D
//            {
//                if (aRxBuffer1[0] == 0x0d)
//                    USART1_RX_STA |= 0x4000;
//                else {
//                    USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = aRxBuffer1[0];
//                    USART1_RX_STA++;
//                    if (USART1_RX_STA > (USART_REC_LEN - 1))
//                        USART1_RX_STA = 0; // 接收数据错误,重新开始接收
//                }
//            }
//        }
//    }
//}
//void USART1_IRQHandler(void)
//{
//    unsigned int timeout = 0;
//    unsigned int maxDelay = 0x1FFFF;

//    HAL_UART_IRQHandler(&huart1); // 调用HAL库中断处理公用函数

//    timeout = 0;
//    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) // 等待就绪
//    {
//        timeout++; // 超时处理
//        if (timeout > maxDelay)
//            break;
//    }

//    timeout = 0;
//    // 一次处理完成之后，重新开启中断并设置RxXferCount为1
//    while (HAL_UART_Receive_IT(&huart1, (unsigned char*)aRxBuffer1, RXBUFFERSIZE) != HAL_OK) {
//        timeout++; // 超时处理
//        if (timeout > maxDelay)
//            break;
//    }
//}
