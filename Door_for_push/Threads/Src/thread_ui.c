#include "thread_ui.h"
#include "function.h"

//#include "Devices/Rtc/rtc.h"
//#include "function.h"
#include "stack.h"
#include "Functions/Inc/list.h"

#include "main.h"
#include "Threads/Inc/thread_faceid.h"
#include "Threads/Inc/thread_fr1002_message_handle.h"

Stack g_stack_ui;
rt_timer_t ui_sleep_timer;
MenuListManager g_main_list, g_rfid_list, g_rfid_list_manager, g_faceid_list_mamager;

int g_ui_timer_flag = 0;

uint8_t s_list_name = 0;



void UiInit(void* args)
{
    //Pop(&g_stack_ui);
    //StackInit(&g_stack_ui, 10);
    //ui_sleep_timer = rt_timer_create("Ui", UiPushUiClock, &g_ui_timer_flag, 1000, RT_TIMER_FLAG_ONE_SHOT);
    //rt_timer_start(ui_sleep_timer);
    AddMenuListNode(&g_main_list, CreateMenuListNode("Manage Password"));
    AddMenuListNode(&g_main_list, CreateMenuListNode("Manage IdCard"));
    AddMenuListNode(&g_main_list, CreateMenuListNode("Manage FaceId"));

    //AddMenuListNode(&g_rfid_list, CreateMenuListNode(CharToString(NumToChar(s_list_name++))));

    AddMenuListNode(&g_rfid_list_manager, CreateMenuListNode("Delete Rfid"));
    AddMenuListNode(&g_rfid_list_manager, CreateMenuListNode("Add Rfid"));

    AddMenuListNode(&g_faceid_list_mamager, CreateMenuListNode("Delete All"));
    AddMenuListNode(&g_faceid_list_mamager, CreateMenuListNode("Add Face"));

    g_stack_ui.stack_max_size = g_stack_ui.stack_remain_size = 20;
    g_stack_ui.sp = g_stack_ui.stack_buf = (void**)StackMalloc(10 * sizeof(void*));
}

void ShowEenterOriginPassWord(void)
{
    LCD_ShowString(1,6,"EenterOrigin",RED,WHITE,16,0);
    //LCD_ShowString(20,56,"PassWord",RED,WHITE,16,0);
}

void ShowEenterNewPassWord(void)
{
    LCD_ShowString(1,6,"EenterNew",RED,WHITE,16,0);
    //LCD_ShowString(20,56,"PassWord",RED,WHITE,16,0);
}

int WaitForNewPassWord(void)
{
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    uint8_t key_pressed;
    while (g_password_buf.effective_lenth != g_password_buf.total_lenth)
    {       
        ShowEenterNewPassWord();
        key_pressed = GetKey();
        switch (key_pressed)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:            
        case 8:            
        case 9:
        {
            InsertPassWord(&g_password_buf, key_pressed);
            break;
        }
        case 11:
        {
            InsertPassWord(&g_password_buf, 0);
            break;
        }  
        case 10:
        {
            /* 删除全部 */
            ClearPassWord(&g_password_buf);
        }
            break;
        case 12:
        {
            /* 删除一位 */
            DeletePassWord(&g_password_buf);
        }
            break;           
        case 13:
        case 14:
        case 15:
        case 16:
        {
            Pop(&g_stack_ui);
            return key_pressed;
        }                               
        default:
            break;
        }
        UiPushUiShowInputPassWord();
    }
    for (int i = 0; i < PASSWORD_LENTH; ++i)
    {
        g_password.data[i] = g_password_buf.data[i];
    }
    ClearPassWord(&g_password_buf);
    //rt_enter_critical();
    FlashWrite(FLASH_PASS_WORD, (uint16_t*)(g_password.data), 2, 0);
    //rt_exit_critical();

}

void ThreadUi(void* args)
{

    UiInit(args);
    UiPushUiClock(NULL);
    while (1)
    {
        
        switch (UiPushUiWaitForPassWord(NULL))
        {
        case 1:
            AddRecoder(&g_door_recoder, k_by_password, GetTimeCounter(&g_systime));
            rt_enter_critical();
            UiPushUiWelcomeWindow();
            rt_exit_critical();
            break;
        case 0:
            rt_enter_critical();
            UiPushUiShowError();
            rt_exit_critical();
            break;
        case 13:
            {              
                switch (UiPushUiShowMenuList(&g_main_list))
                {
                    case 13:
                        {
                            
                            switch (g_main_list.cur_list->offset)
                            {
                            case 0:
                                switch (UiPushUiWaitForPassWord(ShowEenterOriginPassWord))
                                {
                                case 1:
                                    WaitForNewPassWord();
                                    AddRecoder(&g_door_recoder, k_by_password, GetTimeCounter(&g_systime));
                                    LCD_ShowString(5, 40, "Success",RED,WHITE,32,0);
                                    // UiPushUiWelcomeWindow();
                                    break;
                                case 0:
                                    UiPushUiShowError();
                                    break;
                                default:
                                    break;
                                }
                                break;
                            case 1:
                                UiManageRfid();
                                break;
                            case 2:
                                UiManageFaceId();
                                break;
                            
                            default:
                                break;
                            }
                            break;
                        }
                    case 16:
                        UiPushUiShowMenuList(&g_main_list);
                        break;
                    default:
                        break;
                }
            }
            break;
        case 14:
        case 15:
        {
            rt_enter_critical();
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            LCD_ShowString(20,26,"FaceMatching",RED,WHITE,16,0);
            FaceMatchMesageSend();
            //FaceDeletAll();

            uint8_t count = 0;
            while (count < 10 && (g_fr1002_message.isend == 0 || g_fr1002_message.cmd != 0x12))
            {
                DelayUs(1000 * 1000);
                ++count;
            }

            if (g_fr1002_message.isend == 1 && g_fr1002_message.cmd == 0x12)
            {
                if (g_fr1002_message.status == 0)
                {
                    UiPushUiWelcomeWindow();
                    AddRecoder(&g_door_recoder, k_by_faceid, GetTimeCounter(&g_systime));
                }
                else
                {
                    UiPushUiShowError();
                }
                g_fr1002_message.isend = 0;
            }
            else
            {
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(30,26,"TimeOut",RED,WHITE,16,0);
                DelayUs(1000 * 800);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                g_fr1002_message.isend = 0;
            }
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            rt_exit_critical();
        }
        default:
            break;
        }
    }
    

}

#define UI_SP (*(g_stack_ui.sp))
#include "Devices/Rtc/rtc.h"
#include "function.h"
void UiClock(void)
{
    char data[10];
    char data_time[20];
    //rt_timer_stop(ui_sleep_timer);


    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);

    while (1)
    {
        // sb沁恒测试用
        //StackInit(&g_stack_ui, 10);               
        rt_sprintf(data, "%04hu/%02hu/%02hu", g_systime.year, g_systime.month, g_systime.day);
        rt_sprintf(data_time, "%02hu:%02hu:%02hu", g_systime.hour, g_systime.minute, g_systime.second);
        LCD_ShowString(20, 40, data, WHITE, GRAY, 16, 0);
        LCD_ShowString(20, 60, data_time, WHITE, GRAY, 16, 0);
        if (GetKey())
        {
            //rt_timer_start(ui_sleep_timer);
            Pop(&g_stack_ui);
            return;
        }
    }    
}

void UiPushUiClock(void* args)
{
    if (args != NULL)
    {
        if (*(int*)args == 0)
            return;
    }
    Push(&g_stack_ui, &UiClock);
    ((void (*)(void))UI_SP)();
}

#include "function.h"
int UiWaitForPassWord(void(* func)(void))
{
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    uint8_t key_pressed;
    while (1)
    {       
        if (func != NULL) func();
        key_pressed = GetKey();
        switch (key_pressed)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:            
        case 8:            
        case 9:
        {
            InsertPassWord(&g_password_buf, key_pressed);
            break;
        }
        case 11:
        {
            InsertPassWord(&g_password_buf, 0);
            break;
        }  
        case 10:
        {
            /* 删除全部 */
            ClearPassWord(&g_password_buf);
            //g_password_buf.effective_lenth = 0;
        }
            break;
        case 12:
        {
            /* 删除一位 */
            DeletePassWord(&g_password_buf);
        }
            break;           
        case 13:
        case 14:
        case 15:
        case 16:
        {
            Pop(&g_stack_ui);
            return key_pressed;
        }                               
        default:
            break;
        }
        UiPushUiShowInputPassWord();
        int flag = CheckPassWord();
        if (flag == SUCCESS_)
        {
            Pop(&g_stack_ui);
            return SUCCESS_;
        }
        if (flag == ERROR_)
        {
            Pop(&g_stack_ui);
            return ERROR_;
        }
    }
}

int UiPushUiWaitForPassWord(void(* func)(void))
{
    Push(&g_stack_ui, &UiWaitForPassWord);
    return ((int (*)(void(* func)(void)))UI_SP)(func);
}

void UiShowInputPassWord(void)
{
    static int16_t cursor = 1;
    cursor += 512;
    if(cursor > 0) LCD_DrawLine(30+16 * g_password_buf.effective_lenth, 65, 30 + 16*g_password_buf.effective_lenth, 90, BLUE);
    else LCD_DrawLine(30+16 * g_password_buf.effective_lenth, 65, 30+16*g_password_buf.effective_lenth, 90, WHITE);
    LCD_ShowString(30, 60, g_password_buf.data, RED, WHITE, 32, 0);
    LCD_Fill(31 + 16 * (g_password_buf.effective_lenth), 60, 128 , 96 ,WHITE);
    LCD_ShowString(20, 20, "Password:", RED, WHITE, 16, 0);
    Pop(&g_stack_ui);
}


void UiPushUiShowInputPassWord(void)
{
    Push(&g_stack_ui, &UiShowInputPassWord);
    ((void (*)(void))UI_SP)();
}
#include "Beep/beep.h"
void UiWelcomeWindow(void)
{
    /* 刷新*/
    rt_tick_t time = rt_tick_get();
    /* 舵机开启 */
    TIM_SetCompare4(TIM2, 10);
    /*  显示   */
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    while (rt_tick_get() - time < 800)
    {
        LCD_ShowString(5, 40, "Welcome",RED,WHITE,32,0);// 显示Welcome
    }
    PlayOkmusic();
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    TIM_SetCompare4(TIM2, 25);
    Pop(&g_stack_ui);
    return;
}

void UiPushUiWelcomeWindow(void)
{
    //rt_enter_critical();
    Push(&g_stack_ui, &UiWelcomeWindow);
    ((void (*)(void))UI_SP)();
    //rt_exit_critical();
}

void UiShowError(void)
{
    /* 刷新*/
    rt_tick_t time = rt_tick_get();
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    while (rt_tick_get() - time < 800)
    {
        LCD_ShowString(5, 40, "Error",RED,WHITE,32,0);// 显示Welcome
    }
    PlayErrmusic();
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    Pop(&g_stack_ui);
    return;
}

void UiPushUiShowError(void)
{
    //rt_enter_critical();
    Push(&g_stack_ui, &UiShowError);
    ((void (*)(void))UI_SP)();
    //rt_exit_critical();
}

#include "Functions/Inc/list.h"
int UiShowMenuList(MenuListManager* list)
{
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    uint8_t key_pressed;
    while (1)
    {
        switch (list->list_lenth)
        {
            case 0:
            {
                break;
            }
            case 1:
            {
                LCD_ShowString(5,6,list->phead->name,RED,WHITE,16,0);
                break;
            }
            case 2:
            {
                LCD_ShowString(5,6,list->phead->name,RED,WHITE,16,0);
                LCD_ShowString(5,26,list->pend->name,RED,WHITE,16,0);
                break;        
            }
            case 3:
            {
                LCD_ShowString(5,6,list->phead->name,RED,WHITE,16,0);
                LCD_ShowString(5,26,list->pend->prev->name,RED,WHITE,16,0);
                LCD_ShowString(5,46,list->pend->name,RED,WHITE,16,0);
                break;        
            }    
            default:
            {
                if (list->cur_list->offset > 3)
                {
                    LCD_ShowString(5,66,list->pend->name,RED,WHITE,16,0);
                    LCD_ShowString(5,46,list->pend->prev->name,RED,WHITE,16,0);
                    LCD_ShowString(5,26,list->pend->prev->prev->name,RED,WHITE,16,0);
                    LCD_ShowString(5,6,list->pend->prev->prev->prev->name,RED,WHITE,16,0);
                }
                else
                {
                    LCD_ShowString(5,6,list->phead->name,RED,WHITE,16,0);
                    LCD_ShowString(5,26,list->phead->next->name,RED,WHITE,16,0);
                    LCD_ShowString(5,46,list->pend->prev->name,RED,WHITE,16,0);
                    LCD_ShowString(5,66,list->pend->name,RED,WHITE,16,0);
                }
                break;
            }

        }
        for (int i = 0; i < list->list_lenth; ++i)
        {
            if (i != (list->cur_list->offset > 3 ? 3 : list->cur_list->offset))
            {
                LCD_DrawRectangle(2,4 + i * 20 ,126,
                    25 + i * 20 ,WHITE);
            }
            else
            {
                LCD_DrawRectangle(2,4 + i * 20 ,126,
                    25 + i * 20 ,BLUE);
            }
        }
        // LCD_DrawRectangle(2,4 + (list->cur_list->offset > 3 ? 3 : list->cur_list->offset) * 20 ,126,
        //     25 + (list->cur_list->offset > 3 ? 3 : list->cur_list->offset) * 20 ,BLUE);
        key_pressed = GetKey();
        switch (key_pressed)
        {
            case 14:
                list->cur_list = list->cur_list->next;
                break;
            case 15:
                list->cur_list = list->cur_list->prev;
                break;
            case 13:
            case 16:
                Pop(&g_stack_ui);
                return key_pressed;
                break;
            default:
                break;
        }
    }
}

int UiPushUiShowMenuList(MenuListManager* list)
{
    Push(&g_stack_ui, &UiShowMenuList);
    return ((int (*)(MenuListManager*))UI_SP)(list);
}

#include "Functions/Inc/flash.h"
#include "Threads/Inc/thread_rfid.h"
void FlashSaveRfId(RfidManager* rfid)
{
    uint16_t* buf = rt_malloc(sizeof(uint16_t) * (rfid->toal_id_num + 2)); 
    buf[0] = *(uint16_t*)&(rfid->toal_id_num);
    buf[1] = *((uint16_t*)&(rfid->toal_id_num) + 1);
    int offset = FLASH_RFID_OFFSET + 2;
    for (int i = 0; i < rfid->toal_id_num; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            buf[offset + j] = rfid->data[i].id[j];
        }
        offset += FLASH_RFID_OFFSET;
    }
    rt_enter_critical();
    FlashWrite(FLASH_RFID, buf, (rfid->toal_id_num + 2), 1);
    rt_exit_critical();
    rt_free(buf);
}

void UiManageRfid(void)
{
    switch (UiPushUiShowMenuList(&g_rfid_list_manager))
    {
    case 13:
    {
        switch (g_rfid_list_manager.cur_list->offset)
        {
        case 0:
        {
            switch (UiPushUiShowMenuList(&g_rfid_list))
            {
            case 13:
                switch (UiPushUiWaitForPassWord(NULL))
                {
                case SUCCESS_:
                    DeleteIdCard(g_rfid_manager.data[g_rfid_list.cur_list->offset].id);
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    FlashRfidSave();
                    LCD_ShowString(1,26,"DeleteSuccess",RED,WHITE,16,0);
                    rt_enter_critical();
                    DelayUs(1000 * 700);
                    rt_exit_critical();
                    break;
                case ERROR_:
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    LCD_ShowString(1,26,"DeleteFail",RED,WHITE,16,0);
                    rt_enter_critical();
                    DelayUs(1000 * 700);
                    rt_exit_critical();
                default:
                    break;
                }
                break;
            case 16:
                //UiPushUiShowMenuList(&g_rfid_list);
            default:
                break;
            }
            //UiPushUiShowMenuList(&g_rfid_list_manager);
            break;
        }
        case 1:
        {
            switch (UiPushUiWaitForPassWord(ShowEenterOriginPassWord))
            {
            case SUCCESS_:
                rt_enter_critical();
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(1,26,"PutYour",RED,WHITE,16,0);
                LCD_ShowString(20,56,"IdCard",RED,WHITE,16,0);
                uint8_t* tmp = WaitForRfid();
                if (tmp != NULL)
                {
                    AddIdCard(tmp);
                    FlashRfidSave();
                    AddRecoder(&g_door_recoder, k_by_rfid, GetTimeCounter(&g_systime));
                    UiPushUiWelcomeWindow();
                }
                else
                {
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    LCD_ShowString(20,26,"TimeOut",RED,WHITE,32,0);
                }
                rt_exit_critical();
                break;
            case ERROR_:
            {
                rt_enter_critical();
                UiPushUiShowError();
                rt_exit_critical();
                break;
            }
            default:
                break;
            }
            //UiPushUiShowMenuList(&g_rfid_list_manager);
        }

        default:
            break;
        }
        break;
    }
    case 16:
    {
        return;
    }

    }
}

void ShowEenterPassWord(void)
{
    LCD_ShowString(1,6,"EenterYour",RED,WHITE,16,0);
    //LCD_ShowString(20,56,"PassWord",RED,WHITE,16,0);
}
void UiManageFaceId(void)
{
    switch (UiPushUiShowMenuList(&g_faceid_list_mamager))
    {
    case 13:
    {
        uint8_t judge = UiPushUiWaitForPassWord(ShowEenterPassWord);
        switch (g_faceid_list_mamager.cur_list->offset)
        {
        case 0:
        {
            if (judge == SUCCESS_)
            {
                rt_enter_critical();
                FaceDeletAll();
                rt_tick_t time = rt_tick_get();
                while (rt_tick_get() - time < 1200 && g_fr1002_message.isend == 0);
                if (g_fr1002_message.isend == 1)
                {
                    if (g_fr1002_message.cmd = 0x21)
                    {
                        time = rt_tick_get();
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                        LCD_ShowString(20,26,"DelSuccess",RED,WHITE,16,0);
                        while (rt_tick_get() - time < 800);
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    }
                    else
                    {
                        time = rt_tick_get();
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                        LCD_ShowString(20,26,"DelFail",RED,WHITE,16,0);
                        while (rt_tick_get() - time < 800);
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    }
                    g_fr1002_message.isend = 0;
                }
                else
                {
                    time = rt_tick_get();
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    LCD_ShowString(20,26,"TimeOut",RED,WHITE,16,0);
                    while (rt_tick_get() - time < 800);
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                }
                rt_exit_critical();
            }
            else if (judge == ERROR_)
            {
                rt_tick_t time = rt_tick_get();
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"PassWordErr",RED,WHITE,16,0);
                while (rt_tick_get() - time < 800);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            }
        }
        break;
        case 1:
        {
            if (judge == SUCCESS_)
            {
                rt_enter_critical();
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"Adding",RED,WHITE,16,0);
                FaceRegistMessageSend(CharToString(NumToChar(s_list_name)), 1);
                rt_tick_t time = rt_tick_get();
                uint8_t count = 0;
                while (count < 1200 && (g_fr1002_message.isend == 0 || g_fr1002_message.cmd != 0x26))
                {
                    DelayUs(1000 * 10);
                    ++count;
                }

                if (g_fr1002_message.isend == 1 && g_fr1002_message.cmd == 0x26)
                {
                    if (g_fr1002_message.status == 0)
                    {
                        time = rt_tick_get();
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                        LCD_ShowString(20,26,"AddSuccess",RED,WHITE,16,0);
                        while (rt_tick_get() - time < 800);
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    }
                    else
                    {
                        time = rt_tick_get();
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                        LCD_ShowString(20,26,"AddFail",RED,WHITE,16,0);
                        while (rt_tick_get() - time < 800);
                        LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    }
                    g_fr1002_message.isend = 0;
                }
                else
                {
                    time = rt_tick_get();
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                    LCD_ShowString(20,26,"TimeOut",RED,WHITE,16,0);
                    while (rt_tick_get() - time < 800);
                    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                }
                rt_exit_critical();
            }
            else if (judge == ERROR_)
            {
                rt_tick_t time = rt_tick_get();
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"PassWordErr",RED,WHITE,16,0);
                while (rt_tick_get() - time < 800);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            }
        }
        break;
        default:
            break;
        }
    }
    default:
        break;
    }
}
