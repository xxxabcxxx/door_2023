#include "rtc.h"

SystemTime g_systime = {2023, 7, 17, 17, 37, 50, 0};

// 辅助函数：判断一个年份是否为闰年
int IsLeapYear(uint32_t year) 
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 辅助函数：获取指定年份和月份的天数
uint32_t MonthDays(uint32_t year, uint32_t month) 
{
    static const uint32_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint32_t d = days[month - 1];
    if (month == 2 && IsLeapYear(year)) {
        d++;
    }
    return d;
}

// 辅助函数：计算指定年份和月份之前的天数
uint32_t DaysBeforeMonth(uint32_t year, uint32_t month) 
{
    static const uint32_t days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    uint32_t d = days[month - 1];
    if (month > 2 && IsLeapYear(year)) {
        d++;
    }
    return d;
}

uint32_t LeapYearsSince1970(uint32_t year) {
    uint32_t l = (year - 1) / 4 - 1969 / 4;
    uint32_t nl = (year - 1) / 100 - 1969 / 100;
    uint32_t nnl = (year - 1) / 400 - 1969 / 400;
    return l - nl + nnl;
}

void GetTime(SystemTime* time, uint32_t datetime)
{
    uint32_t seconds = datetime % 60;
    uint32_t minutes = (datetime / 60) % 60;
    uint32_t hours = (datetime / (60 * 60)) % 24;
    uint32_t days = (datetime / (24 * 60 * 60));
    uint32_t years = 1970; // RTC时钟默认从1970年1月1日开始计数
    while (days >= (IsLeapYear(years) ? 366 : 365)) {
        days -= IsLeapYear(years) ? 366 : 365;
        years++;
    }
    uint32_t months = 1;
    while (days >= MonthDays(years, months)) {
        days -= MonthDays(years, months);
        months++;
    }
    uint32_t date = days + 1;

    // 将解析出的年月日时分秒值存储到结构体中
    time->year = years;
    time->month = months;
    time->day = date;
    time->hour = hours;
    time->minute = minutes;
    time->second = seconds;
}

uint32_t GetTimeCounter(SystemTime* time)
{
    uint32_t days, seconds;
    uint32_t minutes, hours;
    uint32_t date, month, year;

    // 将日期时间转换为从1970年1月1日开始的秒数
    year = time->year;
    month = time->month;
    date = time->day;
    hours = time->hour;
    minutes = time->minute;
    seconds = time->second;
    days = 365 * (year - 1970) + LeapYearsSince1970(year) + DaysBeforeMonth(year, month) + date - 1;
    seconds += hours * 60 * 60 + minutes * 60 + days * 24 * 60 * 60;

    return seconds;
}

static void RTC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;        //RTC全局中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级1位,从优先级3位
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    //先占优先级0位,从优先级4位
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       //使能该通道中断
  NVIC_Init(&NVIC_InitStructure);       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

uint8_t RTC_Init(void)
{

	//检查是不是第一次配置时钟
	uint8_t temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问



    RTC_ITConfig(RTC_IT_SEC, ENABLE);       //使能RTC秒中断
    RTC_WaitForLastTask();  //等待最近一次对RTC寄存器的写操作完成
    RTC_WaitForSynchro();       //等待RTC寄存器同步

	if (BKP_ReadBackupRegister(BKP_DR1) != RTC_INIT_FLAG)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    {	 			
	    BKP_DeInit();   //复位备份区域
	    RCC_LSEConfig(RCC_LSE_ON);  //设置外部低速晶振(LSE),使用外设低速晶振
	    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)   //检查指定的RCC标志位设置与否,等待低速晶振就绪
	    {
	        temp++;
	        for (volatile int i = 0; i < 1000; ++i);
	    }
	    if(temp>=1000)return 1;//初始化时钟失败,晶振有问题

	    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
	    RCC_RTCCLKCmd(ENABLE);  //使能RTC时钟
	    RTC_WaitForLastTask();  //等待最近一次对RTC寄存器的写操作完成
	    RTC_WaitForSynchro();       //等待RTC寄存器同步

        //RTC_ITConfig(RTC_IT_ALR, ENABLE);		//使能RTC闹钟中断
        //RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        //RTC_WaitForSynchro();		//等待RTC寄存器同步
            
        RTC_EnterConfigMode();/// 允许配置
        RTC_SetPrescaler(32767); //设置RTC预分频的值
        RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
            
        RTC_SetCounter(GetTimeCounter(&g_systime));  //设置时间
        RTC_WaitForLastTask(); 
            
        //RTC_SetAlarm(10+RTC_GetCounter());    //闹钟值设定为当前时间的10秒后
        //RTC_WaitForLastTask();
        
        RTC_ExitConfigMode(); //退出配置模式



        BKP_WriteBackupRegister(BKP_DR1, RTC_INIT_FLAG);	//向指定的后备寄存器中写入用户程序数据
    }
	RTC_NVIC_Config();
}

void RTC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
	{							
		 RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清秒中断
		 GetTime(&g_systime, RTC_GetCounter());  //更新时间
		 RTC_WaitForLastTask();  //等待写入完成
    }
	 
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
	{
	    RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断
	    GetTime(&g_systime, RTC_GetCounter());	//更新时间
	    RTC_WaitForLastTask();		 
        
        RTC_SetAlarm(10+RTC_GetCounter());    //闹钟值设定为当前时间的10秒后
        RTC_WaitForLastTask(); 	
    } 				  								 
}
