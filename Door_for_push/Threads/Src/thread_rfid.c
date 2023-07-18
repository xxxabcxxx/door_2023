#include "thread_rfid.h"

#include "Devices/RFID/rfid.h"
#include "Threads/Inc/thread_ui.h"




void ThreadRfid(void* args)
{
    while (1)
    {
        rt_thread_delay(700);
        switch (ReadRfid())
        {
        case rfid_judge_error:
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            UiPushUiShowError();
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);                
            break;
        case rfid_judge_success:
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
            AddRecoder(&g_door_recoder, k_by_rfid, GetTimeCounter(&g_systime));
            UiPushUiWelcomeWindow();
            LCD_Fill(0,0,LCD_W,LCD_H,WHITE);  
        default:
            break;
        }
    }
}


RfidReadFlag ReadRfid(void)
{
    static unsigned char status_x;
    status_x = PcdRequest(PICC_REQALL,CT);//寻卡
    if(status_x==MI_OK)//寻卡成功
    {
        status_x=MI_ERR;
        status_x = PcdAnticoll(SN);//防冲突
    }

    if (status_x==MI_OK)//防冲突成功
    {
        status_x=MI_ERR;
        status_x =PcdSelect(SN);

    }
    if(status_x==MI_OK)//读卡成功
    {
        u8 i;
        for(i=0;i<4;i++)
        {
            RFID[i*2]=SN[i]/16;
            RFID[i*2]>9?(RFID[i*2]+='7'):(RFID[i*2]+='0');
            RFID[i*2+1]=SN[i]%16;
            RFID[i*2+1]>9?(RFID[i*2+1]+='7'):(RFID[i*2+1]+='0');
        }
        RFID[8]=0;
        int flag = 1;
        //for(i=0;i<8;i++){
        //    if(RFID[i]!=card0[i]){
        //        flag = 0;
        //        break;
        //    }
        //}
        flag = CheckIdCard(RFID);
        status_x=MI_ERR;
        status_x = PcdAuthState(0x60,0x09,KEY,SN);
        if(status_x==MI_OK)//验证成功
        {
            printf("CheckSuc\r\n");
            status_x=MI_ERR;
            status_x=PcdRead(0x08,DATA);
        }
        if(status_x==MI_OK && flag)//验证成功
        {
            return rfid_judge_success;
        }
        else if (status_x==MI_OK)
        {
            return rfid_judge_error;
        }
        else if (status_x == MI_ERR)
            return rfid_read_fail;
    }

    status_x=MI_ERR;
}

uint8_t* WaitForRfid(void)
{
    rt_tick_t tick = rt_tick_get();
    while (rt_tick_get() - tick < 30000)
    {   
        static unsigned char status_x;
        status_x = PcdRequest(PICC_REQALL,CT);//寻卡
        if(status_x==MI_OK)//寻卡成功
        {
            status_x=MI_ERR;
            status_x = PcdAnticoll(SN);//防冲突
        }

        if (status_x==MI_OK)//防冲突成功
        {
            status_x=MI_ERR;
            status_x =PcdSelect(SN);

        }
        if(status_x==MI_OK)//读卡成功
        {
            u8 i;
            for(i=0;i<4;i++)
            {
                RFID[i*2]=SN[i]/16;
                RFID[i*2]>9?(RFID[i*2]+='7'):(RFID[i*2]+='0');
                RFID[i*2+1]=SN[i]%16;
                RFID[i*2+1]>9?(RFID[i*2+1]+='7'):(RFID[i*2+1]+='0');
            }
            RFID[8]=0;

            //for(i=0;i<8;i++){
            //    if(RFID[i]!=card0[i]){
            //        flag = 0;
            //        break;
            //    }
            //}
            return RFID;
        }
    }
    return NULL;
    
}
