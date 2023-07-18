#ifndef MAIN_H_
#define MAIN_H_

#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include "drv_gpio.h"
#include <board.h>

extern rt_thread_t g_thread_ui, g_thread_rfid, g_thread_fr1002_message_handle, g_thread_faceid;
extern rt_sem_t g_rf1002_usart_sem, g_rf1002_message_end_sem, g_faceid_handle_end_sem;

void TIM2_Init( u16 arr, u16 psc, u16 ccp );

#endif
