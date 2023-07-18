#include "blue_tooth.h"

void Bluetooth_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART6, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);


    //UART6 TX PC0  RX PC1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART6, &USART_InitStructure);

    USART_ITConfig(UART6, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = UART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_Cmd(UART6, ENABLE);
}


static void Record_Send(uint8_t *sendData, uint32_t sendLen)
{
    int i=0;
    for(i=0; i<sendLen; i++){
        if (sendData[i] == 0) {
            break;

        }
    USART_SendData(UART6, *(sendData+i));
    while(USART_GetFlagStatus(UART6, USART_FLAG_TXE) == RESET);
    }

}
#include "function.h"
/*volatile*/ uint8_t buf;
void UART6_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void UART6_IRQHandler(void){

    buf = USART_ReceiveData(UART6);
    if(buf=='@'){
        Record_Send("\n", sizeof("\n"));
        SystemTime tmp;
        uint8_t string[30] = {0};

        for (int i = 0; i < g_door_recoder.recode_num; ++i)
        {
            switch (g_door_recoder.data[i].mode)
            {
            case k_by_password:
                Record_Send("ByPassWord", sizeof("ByPassWord"));
                break;
            case k_by_rfid:
                Record_Send("ByRFID", sizeof("ByRFID"));
                break;
            case k_by_faceid:
                Record_Send("ByFaceID", sizeof("ByFaceID"));
                break;
            }
            GetTime(&tmp, g_door_recoder.data[i].time);
            rt_sprintf(string, " %04hu/%02hu/%02hu %02hu:%02hu:%02hu\n",
                    g_systime.year, g_systime.month, g_systime.day, g_systime.hour, g_systime.minute, g_systime.second);
            Record_Send(string, 30);
        }
        if (g_door_recoder.recode_num == 0)
        {
            Record_Send("NoRecorde", sizeof("NoRecorde"));
        }
        //USART_SendData(UART6,'\t');

    }
}
