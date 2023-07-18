#include "function.h"

#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include "drv_gpio.h"

void Tim7ForDelayUsInit(void)
{

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    // 预分频系数
    TIM_TimeBaseInitStructure.TIM_Prescaler = 144 - 1;
    // 自动装载器arr的值
    TIM_TimeBaseInitStructure.TIM_Period = 0x7fff;
    // 
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数方式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器值（仅存在于高级定时器）
    //TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM7, ENABLE);
}

void DelayUs(uint32_t us)
{
    if (us > TIM7->ATRLR)
        DelayUs(us -= TIM7->ATRLR);
    TIM7->CNT = 0;
    while (TIM7->CNT < us % TIM7->ATRLR);
}

#include "Devices/MatrixKey/matrix_key.h"
#include "Devices/BoardKey/boardkey.h"

extern int g_ui_timer_flag;

uint8_t GetKey(void)
{
    uint8_t tmp = GetMartixKeyNum(&g_matrix_key);
    if (tmp) return tmp;
    else tmp = GetBoardKey();
    if (tmp) g_ui_timer_flag = 1;
    return tmp;
}

PassWord g_password = { { '1', '1', '1', '1' }, PASSWORD_LENTH, PASSWORD_LENTH }, 
g_password_buf = {{0}, PASSWORD_LENTH, 0};

int CheckPassWord(void)
{
    if (g_password_buf.effective_lenth == PASSWORD_LENTH)
    {
        while (--g_password_buf.effective_lenth)
        {
            if (g_password.data[g_password_buf.effective_lenth]
            != g_password_buf.data[g_password_buf.effective_lenth])
            {
                ClearPassWord(&g_password_buf);
                return ERROR_;
            }
        }
        ClearPassWord(&g_password_buf);
        return SUCCESS_;
    }
    else return -1;
}




RfidManager g_rfid_manager;

int CheckIdCardSingle(uint8_t* input_id, uint8_t* real_id)
{
    for (int i = 0; i < 8; ++i)
    {
        if (input_id[i] != real_id[i])
            return ERROR_;
    }
    return SUCCESS_;
}

int CheckIdCard(uint8_t* input_id)
{
    int flag = ERROR_;
    for (int i = 0; i < g_rfid_manager.toal_id_num; ++i)
    {
       flag |= CheckIdCardSingle(input_id, g_rfid_manager.data[i].id);
    }
    return flag;
}


void RfidDataCopy(RfidData* target, uint8_t* id, uint32_t offset)
{
    target->offset = offset;
    for (int i = 0; i < 8; ++i)
    {
        target->id[i] = id[i];
    }
}

// 根据数字排序，
// input1 > input2 return 1  
// input1 < input2 return -1 
// input1 = input2 return 0 
int RfidSortJudge(uint8_t* input1, uint8_t* input2)
{
    for (int i = 0; i < 8; ++i)
    {
        if (input1[i] > input2[i])
            return 1;
        else if (input1[i] < input2[i])
            return -1;
    }
    return 0;
}

#include "thread_ui.h"
extern uint8_t s_list_name;
void AddIdCard(uint8_t* input_id)
{
    for (int i = 0; i < g_rfid_manager.toal_id_num; ++i)
    {
        if (RfidSortJudge(input_id, g_rfid_manager.data[i].id) == 0)
            return;
    }
    AddMenuListNode(&g_rfid_list, CreateMenuListNode(CharToString(NumToChar(s_list_name++))));
    if (g_rfid_manager.toal_id_num == 0)
    {
        g_rfid_manager.data = (RfidData*)rt_malloc(sizeof(RfidData) * (g_rfid_manager.toal_id_num + 1));
        RfidDataCopy(g_rfid_manager.data, input_id, 0);
        g_rfid_manager.toal_id_num++;
        return;
    }
    RfidData* tmp = (RfidData*)rt_malloc(sizeof(RfidData) * (g_rfid_manager.toal_id_num + 1));
    for (int i = 0; i < g_rfid_manager.toal_id_num; ++i)
    {
        if (RfidSortJudge(input_id, g_rfid_manager.data[i].id) == 1)
        {
            for (int j = i ; j < g_rfid_manager.toal_id_num; j++)
            {
                RfidDataCopy(&tmp[j + 1], g_rfid_manager.data[j].id, j + 1);
            }
            RfidDataCopy(tmp + i, input_id, i);
            for (int j = 0; j < i; ++j)
            {
                RfidDataCopy(&tmp[j], g_rfid_manager.data[j].id, j);
            }
            rt_free(g_rfid_manager.data);
            g_rfid_manager.data = tmp;
            g_rfid_manager.toal_id_num++;
            return;
        }
        if (i == g_rfid_manager.toal_id_num - 1)
        {
            for (int j = 0; j < g_rfid_manager.toal_id_num; ++j)
            {
                RfidDataCopy(&tmp[j], g_rfid_manager.data[j].id, j);
            }
            RfidDataCopy(tmp + g_rfid_manager.toal_id_num, input_id, g_rfid_manager.toal_id_num);
            rt_free(g_rfid_manager.data);
            g_rfid_manager.data = tmp;
            g_rfid_manager.toal_id_num++;
            return;
        }
    }


}

void DeleteIdCard(uint8_t* input)
{
    RfidData*  tmp = NULL;
    for (int i = 0; i < g_rfid_manager.toal_id_num; ++i)
    {
        if (RfidSortJudge(input, g_rfid_manager.data[i].id) == 0)
        {
            DeleteMenuListNode(&g_rfid_list, i);
            tmp = (RfidData*)rt_malloc(sizeof(RfidData) * (g_rfid_manager.toal_id_num - 1));
            for (int j = 0; j < i; ++j)
            {
                RfidDataCopy(&tmp[j], g_rfid_manager.data[j].id, j);
            }
            for (int j = i + 1; j < g_rfid_manager.toal_id_num; ++j)
            {
                RfidDataCopy(&tmp[j - 1], g_rfid_manager.data[j].id, j - 1);
            }
        }
    }
    g_rfid_manager.toal_id_num--;
    rt_free(g_rfid_manager.data);
    g_rfid_manager.data = tmp;
}

void FlashRfidSave(void)
{
    FlashWrite(FLASH_RFID, (uint16_t*)(&(g_rfid_manager.toal_id_num)), 2, 0);
    uint16_t* ptr = (uint16_t*)FLASH_RFID;
    ptr = ptr + 2 + FLASH_RFID_OFFSET;
    for (int i = 0; i < g_rfid_manager.toal_id_num; ++i)
    {
        FlashWrite((uint32_t)ptr, (uint16_t*)(&(g_rfid_manager.data[i].id)), 4, 0);
        ptr += FLASH_RFID_OFFSET;
    }
}

// void TIM7_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
// void TIM7_IRQHandler(void)
// {

// }
Recoder g_door_recoder;

void AddRecoder(Recoder* recode, OpenMode mode, uint32_t time)
{
    RecodeData* tmp;
    if (recode->recode_num >= MAX_RECODEDATA_NUM)
    {
        tmp = rt_malloc(sizeof(RecodeData) * MAX_RECODEDATA_NUM);
        for (int i = 1; i < MAX_RECODEDATA_NUM; ++i)
        {
            tmp[i - 1] = recode->data[i];
        }
        tmp[MAX_RECODEDATA_NUM - 1].mode = mode;
        tmp[MAX_RECODEDATA_NUM - 1].time = time;        
    }
    else
    {
        tmp = rt_malloc(sizeof(RecodeData) * (recode->recode_num + 1));
        for (int i = 0; i < recode->recode_num; ++i)
        {
            tmp[i] = recode->data[i];
        }
        tmp[recode->recode_num].mode = mode;
        tmp[recode->recode_num].time = time;        
        recode->recode_num++;
    }
    if (recode->data != NULL)
    {
        rt_free(recode->data);
    }
    recode->data = tmp;
    FlashSaveRecode();    
}

void FlashSaveRecode(void)
{
    FlashWrite(FLASH_RECODE, (uint16_t*)&(g_door_recoder.recode_num), FLASH_RECODE_OFFSET, 0);
    uint16_t* ptr = (uint16_t*)FLASH_RECODE + FLASH_RECODE_OFFSET;
    for (int i = 0; i < g_door_recoder.recode_num; ++i)
    {
        FlashWrite((uint32_t)ptr, (uint16_t*)(g_door_recoder.data + i), FLASH_RECODE_OFFSET * 2, 0);
        ptr += FLASH_RECODE_OFFSET * 2;
    }
}
