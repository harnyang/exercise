#include "usart_priv.h"

/* 用于保存接受状态和接受数据存放的地址,并且对USART句柄处理类型进行了定义 */
uint16_t g_usart_rx_stat=0;
uint8_t g_rx_buffer[RXBUFFERSIZE]={0};
USART_HandleTypeDef USART_HandleStructure;

uint8_t g_usart_rx_buf[USART_RECIVE_LENGTH];

void usart_init(uint32_t BaudRate)
{
    USART_InitTypeDef USART1_InitStructure;
    USART1_InitStructure.BaudRate=BaudRate;
    USART1_InitStructure.CLKLastBit=
    USART_HandleStructure.Init.BaudRate=BaudRate;
    USART_HandleStructure.Instance=USART_UX;
    USART_HandleStructure.Init.Mode=UART_MODE_TX_RX;
    USART_HandleStructure.Init.Parity=UART_PARITY_NONE;
    USART_HandleStructure.Init.StopBits=UART_STOPBITS_1;
    USART_HandleStructure.Init.WordLength=UART_WORDLENGTH_8B;

    HAL_USART_Init(&USART_HandleStructure);
    HAL_UART_Receive_IT(&USART_HandleStructure,ux_rx_buffer,RXBUFFERSIZE);
}
void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{
    //初始化GPIO：
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode=GPIO_MODE_AF_INPUT;
    GPIO_InitStructure.Pin=USART1_RX_GPIO_PIN;
    GPIO_InitStructure.Pull=GPIO_PULLUP;//这里选择上拉的原因是上拉不会影响状态的输入，只是让不确定状态的高阻态X变为有确定状态的输入。而下面的输出同样也是设置的上拉电阻因为这样可以提高输出电平
    //而输出方式的选择虽然开漏输出加电阻和推挽式输出的效果一样但是还是有一定的优缺点的可以从功耗，驱动力，和开关速度来说。
    GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT,&GPIO_InitStructure);

    GPIO_InitStructure.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pin=USART1_TX_GPIO_PIN;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT,&GPIO_InitStructure);

    USART1_RX_GPIO_ENABLE();
    USART1_TX_GPIO_ENABLE();

    __HAL_RCC_USART1_CLK_ENABLE();

    HAL_NVIC_SetPriority(USART1_IRQn,2,2);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

}

void USART_UX_IRQHandler();
{   
    HAL_UART_IRQHandler(&USART_HandleStructure); //调用HAL库的中断处理函数里面会对接受进行回调

    while (HAL_UART_Receive_IT(&USART_HandleStructure,ux_rx_buffer,RXBUFFERSIZE)!=HAL_OK)
    {
        
    }
    
}
//在 HAL_UART_IRQHadnler()之中调用的UART_RECIVE_IT之中被调用//_需要注意的是这些函数都不需要声明因为官方已经声明过了。
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance==USART_UX)
    {
        if ((g_usart_rx_stat & 0x8000) == 0)//如果接受未完成
        {
            if ((g_usart_rx_stat & 0x4000)==1) //如果接收到回车键；
            {
                if (g_rx_buffer[0] == 0x0a)//如果接受到的是换行减
                {
                    g_usart_rx_stat |=0x8000;
                }
                else
                {
                    g_usart_rx_stat=0;
                }
            }
            else
            {
                if (g_rx_buffer[0] == 0x0d)//如果接收到的是回车键
                {
                    g_usart_rx_stat |=0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_stat & 0x3FFF] = g_rx_buffe[0]
                    g_usart_rx_stat++;

                    if (g_usart_rx_stat > (USART_RECIVE_LENGTH - 1))
                    {
                        g_usart_rx_stat = 0;             /* 接收数据错误,重新开始接收 */
                    }
                }
                
            }
            
        }
        
    }
    
}
