#include "boardkey.h"

void InitBoardKey(void)
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct;
    GPIO_InitTypdefStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitTypdefStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitTypdefStruct);
}

uint8_t GetBoardKey(void)
{
    uint8_t tmp = 0;
    {
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0);
            tmp =  13;
        }
    }
    {
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0);
            tmp =  14;
        }
    }
    {
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == 0);
            tmp =  15;
        }
    }
    {
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == 0);
            tmp =  16;
        }
    }
    return tmp;
}
