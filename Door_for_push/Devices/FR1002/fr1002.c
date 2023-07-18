#include "fr1002.h"
#include "main.h"


Fr1002User g_receive_user;

void FR1002Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure={0};


    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);


    //USART3 TX PD8  RX PD9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    //Wake up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* GPIOA ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //按下为高电平，用上升沿
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_Cmd(USART3, ENABLE);
}

void FaceMatchMesageSend(void)
{
    uint8_t cmd[] = { 0xEF, 0xAA, 0x12, 0x00, 0x02, 0x00, 0x03, 0x13 };
    MSG_Send(cmd, sizeof(cmd));
}

static unsigned char GetParityCheck(uint8_t *p, int len)
{
    uint8_t nParityCheck = 0;
    for (int i = 2; i < len - 1; ++i)
    {
        nParityCheck ^= p[i];
    }

    return nParityCheck;
}

void FaceRegistMessageSend(uint8_t* name, uint8_t lenth)
{
    uint8_t cmd[] = { 0xEF, 0xAA, 0x26, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x0A,
           0x00, 0x00, 0x00, 0x1E };
    lenth %= 32;
    cmd[5] = 0;
    cmd[38] = 0x01;
    rt_memcpy(cmd + 6, name, lenth);
    // 单向注册
    cmd[39] = 0x01;
    // 超时时间
    cmd[41] = 0x0a;
    cmd[45] = GetParityCheck(cmd, sizeof(cmd));
    MSG_Send(cmd, sizeof(cmd));
}

void FaceDeletById(uint8_t id)
{

}

void FaceDeletAll(void)
{
    uint8_t cmd[6] = { 0xEF, 0xAA, 0x21, 0x00, 0x00, 0x21 };
    MSG_Send(cmd, sizeof(cmd));
}

uint8_t VerifyParity(uint8_t data,  Fr1002Message* message)
{
    uint8_t nParityCheck = 0;
    nParityCheck ^= *(uint8_t*)&(message->data_lenth);
    nParityCheck ^= *((uint8_t*)&(message->data_lenth) + 1);
    nParityCheck ^= message->cmd;
    nParityCheck ^= message->status;

    for (uint32_t i = 0; i < message->data_lenth; i++)
        nParityCheck ^= message->data[i];
    message->parity = nParityCheck;
    return GET_HEAD;
}

void MSG_Send(uint8_t *sendData, uint32_t sendLen)
{
    int i=0;
    for(i=0; i<sendLen; i++){
    USART_SendData(USART3, *(sendData+i));
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}
