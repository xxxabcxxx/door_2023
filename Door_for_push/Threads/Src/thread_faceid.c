#include "thread_faceid.h"
#include "main.h"
#include "Devices/FR1002/fr1002.h"


#include "Threads/Inc/main.h"
#include "Threads/Inc/thread_rfid.h"
#include "Threads/Inc/thread_ui.h"
#include "thread_fr1002_message_handle.h"
#include "Threads/Inc/thread_faceid.h"
#include "Functions/Inc/function.h"
#include "Functions/Inc/flash.h"
void ThreadFaceId(void* args)
{
    while (1)
    {
        rt_sem_take(g_rf1002_message_end_sem, RT_WAITING_FOREVER);
        //rt_enter_critical();
        switch (g_fr1002_message.cmd)
        {
        case 0x12:
        {
            if (g_fr1002_message.data_lenth == 0x26
                    && g_fr1002_message.status == 0)
            {
                g_receive_user.id[0] = g_fr1002_message.data[0];
                g_receive_user.id[1] = g_fr1002_message.data[1];
                for (int i = 0; i < 32; ++i)
                {
                    g_receive_user.name[i] = g_fr1002_message.data[i + 2];
                }
                UiPushUiWelcomeWindow();
                AddRecoder(&g_door_recoder, k_by_faceid, GetTimeCounter(&g_systime));
                g_fr1002_message.isend = 0;
            }
            else
            {
                UiPushUiShowError();
            }
            
            break;
        }
        case 0x26:
        {
            if (g_fr1002_message.data_lenth == 0x05
                    && g_fr1002_message.status == 0)
            {
                g_receive_user.id[0] = g_fr1002_message.data[0];
                g_receive_user.id[1] = g_fr1002_message.data[1];
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"AddSuccess",RED,WHITE,32,0);
                DelayUs(1000 * 700);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            }
            else
            {
                UiPushUiShowError();
            }
            rt_sem_release(g_faceid_handle_end_sem);
        }
        break;
        case 0x21:
        {
            if (g_fr1002_message.data_lenth == 0x02
                    && g_fr1002_message.status == 0)
            {
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"DelSuccess",RED,WHITE,32,0);
                DelayUs(1000 * 700);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            }
            else
            {
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
                LCD_ShowString(20,26,"DelFailed",RED,WHITE,32,0);
                DelayUs(1000 * 700);
                LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            }
            rt_sem_release(g_faceid_handle_end_sem);
            break;
        }
        default:
            break;
        }
        
        //rt_exit_critical();
    }

}
