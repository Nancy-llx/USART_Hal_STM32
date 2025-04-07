# USART_Hal_STM32

 Implementing USART Communication Using HAL in STM32

核心文件为：

user_usart.c

user_usart.h

示例工程为：

F401_OLED_USART_NVIC_DMA

## 说明

通过Hal库实现USART串口通信

采用中断+DMA实现不定长数据接收

数据发送有阻塞式（printf重定向）和非阻塞式（DMA）两种方式

## 备注

- 在一些串口通信软件中，中文会导致乱码

- 该库高度依赖串口usart1，难以扩展到其他串口，仍需改进

    
