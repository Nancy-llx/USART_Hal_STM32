/*
*********************************************************************************************************
*
*	模块名称 : 串口收发驱动模块Hal库版本
*	文件名称 : user_usart.h
*	版    本 : V2.0
*	说    明 : 头文件
* 编辑时间 ：2025/4/7
*	作    者 ：LLX
* 备    注 ：使用中断+DMA
*            目前只能初始化一个串口，待改进
*            目前主要针对USART1，可扩展性待进一步改进
*            仍不支持中文
*********************************************************************************************************
*/

#include "user_usart.h"
#include <stdarg.h> // For va_list, va_start, va_end

// 接收缓冲区 (设为 static，限制作用域在本文件)
static uint8_t USART1_RX_BUF[USART_REC_LEN];


/**
 * @brief  启动指定串口的 DMA 空闲中断接收
 * @param  huart: UART handle
 * @param  pData: 指向接收缓冲区的指针
 * @param  Size:  接收缓冲区的最大长度
 * @retval None
 */
void User_USART_Start_DMA_Receive(UART_HandleTypeDef *huart)
{
    // 确保传入的指针和大小有效
    if (huart == NULL )
    {
        // 可以添加错误处理逻辑，例如打印错误信息或断言
        return;
    }
    // 启动 DMA 接收，使用空闲中断模式
    HAL_UARTEx_ReceiveToIdle_DMA(huart, USART1_RX_BUF, USART_REC_LEN);
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


/**
 * @brief  uart数据接收回调函数，处理传输完成中断和传输过半中断
 * @param  huart: UART handle
 * @param  Size:  接收到的数据长度
 * @retval None
 *         利用DMA空闲中断实现不定长数据接收
 *         传输过半中断同样会触发该回调函数，需要进行判断
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart->Instance == USART1)
  {
    if(huart->RxEventType == HAL_UART_RXEVENT_IDLE||huart->RxEventType == HAL_UART_RXEVENT_TC)//idle空闲，tc超出数据量，ht传输过半
    {
      // --- 在这里处理接收到的数据 ---
      // Size 参数表示本次接收到的有效数据长度
      // 数据存储在 USART1_RX_BUF[0] 到 USART1_RX_BUF[Size-1]

      // 示例：简单回显接收到的数据
      // 注意：如果数据处理时间过长，应考虑使用标志位通知主循环处理
      HAL_UART_Transmit_DMA(&huart1, USART1_RX_BUF, Size);//发送接收到的数据    
      // --- 清零接收缓冲区 (可选，取决于应用需求) ---
      // 如果不清零，下次接收的数据会覆盖旧数据，但 Size 会指示新数据的长度
      // memset(USART1_RX_BUF, 0, Size); // 只清零接收到的部分
      // memset(USART1_RX_BUF, 0, USART_REC_LEN); // 清零整个缓冲区

      // --- 重新启动 DMA 接收 ---
      // 必须重新启动，否则不会再接收数据
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, USART1_RX_BUF, USART_REC_LEN);//为下次接收开启中断，否则只会执行一次 
      //可以增加标志位说明此处传输完成，传递到主函数
    }    
  }
}

/*---------------------------------printf函数配置-----------------------------------*/
 
/* 使用microLib的方法，记得要在魔术棒哪里进行勾选 */
//重定向c库函数printf到串口，重定向后可使用printf函数

#ifdef __GNUC__            //gcc编译器宏定义
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/*上面的意思是：
如果定义了宏__GNUC__，即使用GCC编译器，则定义宏#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
如果没有定义宏__GNUC__，即不使用GCC编译器，则定义宏#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)  */
 
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);//若要改为其他串口直接修改USART1、2
    return ch;
}

/*---------------------------------可选：封装 printf-----------------------------------*/

/**
 * @brief  封装一个 printf 函数，可以直接指定串口发送 (内部使用 DMA 或 阻塞发送)
 *         注意：这个实现使用了较大的栈空间，并且是阻塞等待格式化完成。
 * @param  huart: UART handle
 * @param  format: 格式化字符串
 * @param  ...: 可变参数
 * @retval None
 */
void User_USART_Printf(UART_HandleTypeDef *huart, const char *format, ...)
{
    if (huart == NULL || format == NULL) {
        return;
    }

    char buffer[256]; // 临时缓冲区，用于存储格式化后的字符串，大小根据需要调整
    va_list args;
    int len;

    va_start(args, format);
    // 使用 vsnprintf 防止缓冲区溢出
    len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len > 0)
    {
        // 可以选择使用 DMA 发送或阻塞发送
        // 1. DMA 发送 (非阻塞，效率高，但需要注意 buffer 的生命周期)
        //    如果 buffer 是局部变量，DMA 可能在函数返回后还在传输，导致数据错误
        //    需要确保 DMA 传输完成，或者使用静态/全局 buffer。
        //    为简单起见，这里使用阻塞发送。
        // User_USART_Transmit_DMA(huart, (uint8_t*)buffer, len);

        // 2. 阻塞发送 (简单可靠)
        HAL_UART_Transmit(huart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
    }
}

////初始化程序
//void User_USART1_Init(UART_HandleTypeDef *huart1)
//{
//  HAL_UARTEx_ReceiveToIdle_DMA(huart1, USART1_RX_BUF, USART_REC_LEN);//开启接收数据中断（DMA）
//}


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



//不再重定向fgetc，阻塞式的fgetc与dma接收机制存在冲突，直接在会回调函数中处理接收到的数据即可
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
