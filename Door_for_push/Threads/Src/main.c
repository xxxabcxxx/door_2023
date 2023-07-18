/********************************** (C) COPYRIGHT *******************************
#include <Threads/Src/thread_faceid.c>
#include <thread_fr1002_message_handle.h>
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "Devices/Beep/beep.h"
#include "Devices/LCD/lcd.h"
#include "Devices/MatrixKey/matrix_key.h"
#include "Devices/Rtc/rtc.h"
#include "Devices/BoardKey/boardkey.h"
#include "Devices/RFID/rfid.h"
#include "Devices/BlueTooth/blue_tooth.h"

#include "Threads/Inc/main.h"
#include "Threads/Inc/thread_rfid.h"
#include "Threads/Inc/thread_ui.h"
#include "thread_fr1002_message_handle.h"
#include "Threads/Inc/thread_faceid.h"
#include "Functions/Inc/function.h"
#include "Functions/Inc/flash.h"
/* Global typedef */

/* Global define */


#define LED0_PIN  GET_PIN(E,11)     //PE11
static inline void StartThread(rt_thread_t thread)
{
    if (thread->stack_size) rt_thread_startup(thread);
}
static inline void InitPassWord(void)
{
    FlashRead(FLASH_PASS_WORD, (uint16_t*)(g_password_buf.data), 2);
    if (g_password_buf.data[0] != FLASH_ORIGIN_8)
    {
        FlashRead(FLASH_PASS_WORD, (uint16_t*)(g_password.data), 2);
    }
    ClearPassWord(&g_password_buf);
}
static inline void InitRfid(void)
{
    FlashRead(FLASH_RFID, (uint16_t*)(&(g_rfid_manager.toal_id_num)), 2);
    if (g_rfid_manager.toal_id_num == FLASH_ORIGIN_32)
    {
        g_rfid_manager.toal_id_num = 0;
        AddIdCard(card0);
    }
    else
    {
        uint16_t* ptr = (uint16_t*)FLASH_RFID;
        ptr = ptr + 2 + FLASH_RFID_OFFSET;
        int n = g_rfid_manager.toal_id_num;
        g_rfid_manager.toal_id_num = 0;
        for (int i = 0; i < n; ++i)
        {
             AddIdCard((uint8_t*)ptr);
             ptr += FLASH_RFID_OFFSET;
        }
    }
}
static inline void InitRecode(void)
{
    FlashRead(FLASH_RECODE, (uint16_t*)&(g_door_recoder.recode_num), FLASH_RECODE_OFFSET);
    if (g_door_recoder.recode_num == FLASH_ORIGIN_32)
    {
        g_door_recoder.recode_num = 0;
        return;
    }
    uint16_t* ptr = (uint16_t*)FLASH_RECODE + FLASH_RECODE_OFFSET;
    g_door_recoder.data = rt_malloc(sizeof(RecodeData) * g_door_recoder.recode_num);
    for (int i = 0; i < g_door_recoder.recode_num; ++i)
    {
        FlashRead((uint32_t)ptr, (uint16_t*)(g_door_recoder.data + i), FLASH_RECODE_OFFSET * 2);
        ptr += FLASH_RECODE_OFFSET * 2;
    }
}

/* Global Variable */
//int test = 0;
//int gc = 0;
rt_thread_t g_thread_ui, g_thread_rfid, g_thread_fr1002_message_handle, g_thread_faceid;
rt_sem_t g_rf1002_usart_sem, g_rf1002_message_end_sem, g_faceid_handle_end_sem;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    rt_enter_critical();

    Tim7ForDelayUsInit();
    TIM2_Init(200-1,14400-1,25);

    //TIM2->CNT = 0;
    //while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == SET)
    //{
    //    gc++;
    //}
    //DelayUs(1000 * 20);
    //test = TIM2->CNT;
    Bluetooth_Init();
    BeepInit();
    LCD_Init();
    RTC_Init();
    InitMatrixKey(&g_matrix_key);
    RFID_Init();
    InitBoardKey();
    FR1002Init();
    InitRfid();
    InitPassWord();
    InitRecode();
    DelayUs(1000);

    rt_exit_critical();
    g_thread_ui = rt_thread_create("ThreadUi", &ThreadUi, NULL, 1024, 8, 2);
    StartThread(g_thread_ui);

    g_thread_rfid = rt_thread_create("ThreadId", &ThreadRfid, NULL, 512, 7, 1);
    StartThread(g_thread_rfid);

    g_rf1002_usart_sem = rt_sem_create("face", 0, RT_IPC_FLAG_PRIO);

    g_rf1002_message_end_sem = rt_sem_create("faceId", 0, RT_IPC_FLAG_PRIO);

    g_faceid_handle_end_sem = rt_sem_create("faceEnd", 0, RT_IPC_FLAG_PRIO);

    g_thread_faceid = rt_thread_create("ThreadFaceId", &ThreadFaceId, NULL, 512, 6, 1);
    StartThread(g_thread_faceid);

    g_thread_fr1002_message_handle = rt_thread_create("ThreadFace", &ThreadFr1002MessageHandle, NULL, 512, 6, 1);
    StartThread(g_thread_fr1002_message_handle);

    rt_thread_delete(rt_thread_self());





    // rt_kprintf("MCU: CH32V307\n");
	// rt_kprintf("SysClk: %dHz\n",SystemCoreClock);
    // rt_kprintf("www.wch.cn\n");
	// LED1_BLINK_INIT();

	// GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	// while(1)
	// {
	//     GPIO_SetBits(GPIOE,GPIO_Pin_12);
	//     rt_thread_mdelay(500);
	//     GPIO_ResetBits(GPIOE,GPIO_Pin_12);
	//     rt_thread_mdelay(500);
	// }
}


void TIM2_Init( u16 arr, u16 psc, u16 ccp )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);

    /* TIM2_CH4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;     //高电平有效
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  //停止时为低电平
    TIM_OC4Init( TIM2, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs(TIM2, ENABLE );
    TIM_OC4PreloadConfig( TIM2, TIM_OCPreload_Enable );
    TIM_Cmd( TIM2, ENABLE );
}
/*********************************************************************
 * @fn      led
 *
 * @brief   gpio operation by pins driver.
 *
 * @return  none
 */






