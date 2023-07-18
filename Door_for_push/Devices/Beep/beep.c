#include "beep.h"



void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    /* Beep */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
}

const uint16_t tone[14] = {247,262,294,330,349,392,440,494,523,587,659,698,784,1000};//音频数据表
#include "function.h"
void Sound(u16 frq)
{
    u32 time;
    if(frq != 1000)
    {
        time = 100000/((u32)frq);;
        GPIO_SetBits(GPIOA, GPIO_Pin_1);//打开蜂鸣器

        DelayUs(time);
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);//关闭蜂鸣器
        DelayUs(time);
    }else
        DelayUs(1000);
}

void PlayOkmusic(void)
{
  u8 music[]={12,6};
  u8 time[] ={4,4};
    u32 yanshi;
    u16 i,e;
    yanshi = 2;//10;  4;  2
    for(i=0;i<sizeof(music)/sizeof(music[0]);i++){
        for(e=0;e<((u16)time[i])*tone[music[i]]/yanshi;e++){
            Sound((u32)tone[music[i]]);
        }
    }
}

void PlayErrmusic(void)
{
  u8 music[]={1,1};
  u8 time[] ={4,4};
    u32 yanshi;
    u16 i,e;
    yanshi = 2;//10;  4;  2
    for(i=0;i<sizeof(music)/sizeof(music[0]);i++){
        for(e=0;e<((u16)time[i])*tone[music[i]]/yanshi;e++){
            Sound((u32)tone[music[i]]);
        }
    }
}