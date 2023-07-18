#ifndef RTC_H_
#define RTC_H_
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include "drv_gpio.h"
#include <board.h>

#define RTC_INIT_FLAG 0x5556

typedef struct SystemTime{
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint16_t dayOfWeek;
}SystemTime;

extern SystemTime g_systime;

// 从SystemTime结构体获取总秒数
uint32_t GetTimeCounter(SystemTime* time);
// 从总秒数解析Systemtime结构体
void GetTime(SystemTime* time, uint32_t datetime);


void RTC_IRQHandler(void);
uint8_t RTC_Init(void);

#endif
