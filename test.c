/*
*********************************************************************************************************
*
*	模块名称 : 串口收发驱动模块 (HAL版)
*	文件名称 : user_usart.c
*	版    本 : V2.0
*	说    明 : 实现文件
* 编辑时间 ：2025/4/7
*	作    者 ：(你的名字)
* 备    注 ：使用 HAL 库，中断+DMA接收，printf重定向
*            移除了与DMA接收冲突的fgetc重定向
*            目前主要针对USART1，可扩展性待进一步改进
*********************************************************************************************************
*/

#include "user_usart.h"
#include <stdarg.h> // For va_list, va_start, va_end

// --- 全局/静态变量 ---

// 接收缓冲区 (设为 static，限制作用域在本文件)
static uint8_t USART1_RX_BUF[USART_REC_LEN];

// printf 重定向的目标串口句柄 (设为 static)
static UART_HandleTypeDef *g_huart_printf = NULL;

// --- 函数定义 ---

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
 * @brief  使用 DMA 非阻塞发送数据
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
 * @brief  设置用于 printf 重定向的串口
 * @param  huart: 指向要用于 printf 输出的 UART Handle
 * @retval None
 */
void User_USART_Set_Printf_Uart(UART_HandleTypeDef *huart)
{
    g_huart_printf = huart;
}

/*
*********************************************************************************************************
*	函 数 名: HAL_UARTEx_RxEventCallback
*	功能说明: UART DMA 接收事件回调函数 (包括 IDLE, TC, HT)
*	形    参: huart: 发生事件的 UART handle
*			  Size:  接收到的数据大小 (对于 IDLE 事件，这是上次 DMA 传输启动到总线空闲时接收的字节数)
*	返 回 值: None
* 备    注：利用DMA空闲中断实现不定长数据接收
*********************************************************************************************************
*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // --- 处理 USART1 的接收事件 ---
    if (huart->Instance == USART1) // 使用 Instance 比较更可靠
    {
        // 判断是否是 IDLE 或 TC (Transfer Complete) 事件
        // TC 通常发生在缓冲区满时，IDLE 发生在总线空闲时
        if (huart->RxEventType == HAL_UART_RXEVENT_IDLE || huart->RxEventType == HAL_UART_RXEVENT_TC)
        {
            // --- 在这里处理接收到的数据 ---
            // Size 参数表示本次接收到的有效数据长度
            // 数据存储在 USART1_RX_BUF[0] 到 USART1_RX_BUF[Size-1]

            // 示例：简单回显接收到的数据
            // 注意：如果数据处理时间过长，应考虑使用标志位通知主循环处理
            User_USART_Transmit_DMA(&huart1, USART1_RX_BUF, Size); // 使用封装的函数发送

            // --- 清零接收缓冲区 (可选，取决于应用需求) ---
            // 如果不清零，下次接收的数据会覆盖旧数据，但 Size 会指示新数据的长度
            // memset(USART1_RX_BUF, 0, Size); // 只清零接收到的部分
            // memset(USART1_RX_BUF, 0, USART_REC_LEN); // 清零整个缓冲区

            // --- 重新启动 DMA 接收 ---
            // 必须重新启动，否则不会再接收数据
            User_USART_Start_DMA_Receive(&huart1, USART1_RX_BUF, USART_REC_LEN); // 使用封装的函数启动
        }
        // 可以选择性处理 HT (Half Transfer) 事件，但不在此示例中处理
        // else if (huart->RxEventType == HAL_UART_RXEVENT_HT) { ... }
    }

    // --- 处理其他串口的接收事件 (如果需要) ---
    /*
    else if (huart->Instance == USART2)
    {
        // 处理 USART2 的逻辑...
        // 需要为 USART2 定义缓冲区和处理方式
        // static uint8_t USART2_RX_BUF[USART_REC_LEN];
        // if (huart->RxEventType == HAL_UART_RXEVENT_IDLE || huart->RxEventType == HAL_UART_RXEVENT_TC)
        // {
        //     // ... 处理 USART2 数据 ...
        //     User_USART_Start_DMA_Receive(&huart2, USART2_RX_BUF, USART_REC_LEN);
        // }
    }
    */
}

/*---------------------------------printf 函数重定向-----------------------------------*/

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  重定向 C 库函数 printf 到指定的串口 (阻塞式发送)
 * @param  ch: 要发送的字符
 * @param  f: 文件指针 (未使用)
 * @retval 返回发送的字符
 */
PUTCHAR_PROTOTYPE
{
    if (g_huart_printf == NULL) {
        return -1; // 如果没有设置目标串口，则不发送
    }

    // 使用 HAL 库的阻塞式发送函数发送单个字符
    // HAL_MAX_DELAY 表示一直等待直到发送完成或超时 (实际上 HAL 实现中通常是无限等待)
    HAL_StatusTypeDef status = HAL_UART_Transmit(g_huart_printf, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        return ch;
    } else {
        return -1; // 发送失败
    }
}

/*---------------------------------fgetc / scanf 重定向 (已移除)-----------------------------------*/
/*
 * 不再重定向 fgetc。
 * 原因：阻塞式的 fgetc 与非阻塞的 DMA+IDLE 接收机制存在冲突。
 *      DMA 在后台接收数据块，而 fgetc 会阻塞等待单个字符，导致逻辑混乱。
 * 替代方案：在 HAL_UARTEx_RxEventCallback 回调函数中处理接收到的数据 (USARTx_RX_BUF)，
 *          或者设置标志位，由主循环来解析缓冲区中的数据。
 */
/*
// int fgetc(FILE *f)
// {
//     // 此函数与 DMA IDLE 接收冲突，已移除
// }
*/


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