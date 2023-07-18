#include "thread_fr1002_message_handle.h"

uint8_t g_fr1002_receive_buf[RF1002_RECEIVE_BUF_MAX];
uint8_t g_buf_store_offset = 0;
uint8_t g_buf_handle_offset = 0;
uint8_t g_fr1002_status = GET_HEAD;
uint8_t g_mesage_effect_offset = 0;
int32_t g_fr1002_error_count = 0;

#define __FR1002ERROR_HANDLE()                \
do {                                          \
    g_mesage_effect_offset = 0;               \
    g_fr1002_status = GET_HEAD;               \
    ++g_fr1002_error_count;                   \
} while(0)


Fr1002Message g_fr1002_message;

void ThreadFr1002MessageHandle(void* args)
{
    while (1)
    {
        rt_sem_take(g_rf1002_usart_sem, RT_WAITING_FOREVER);
        while (g_buf_handle_offset < g_buf_store_offset)
        {
            switch (g_fr1002_status)
            {
            case GET_HEAD:
            {
                if (g_mesage_effect_offset == 0)
                {
                    if (g_fr1002_receive_buf[g_buf_handle_offset] == 0xef)
                    {
                        ++g_mesage_effect_offset;
                        g_fr1002_status = GET_HEAD;
                    }
                }
                else
                {
                    if (g_fr1002_receive_buf[g_buf_handle_offset] == 0xaa
                    && g_mesage_effect_offset == 1)
                    {
                        ++g_mesage_effect_offset;
                        g_fr1002_status = GET_MESSAGE_TYPE;
                    }
                    else
                    {
                        __FR1002ERROR_HANDLE();
                    }
                }
                break;
            }
            case GET_MESSAGE_TYPE:
            {
                if (g_mesage_effect_offset == 2)
                {
                    g_fr1002_status = GET_LENGTH;
                    ++g_mesage_effect_offset;
                }
                break;
            }
            case GET_LENGTH:
            {
                if (g_mesage_effect_offset == 3)
                {
                    *((uint8_t*)&(g_fr1002_message.data_lenth) + 1 )
                    = g_fr1002_receive_buf[g_buf_handle_offset];
                    ++g_mesage_effect_offset;
                }
                else if (g_mesage_effect_offset == 4)
                {
                    *((uint8_t*)&(g_fr1002_message.data_lenth))
                    = g_fr1002_receive_buf[g_buf_handle_offset];
                    ++g_mesage_effect_offset;
                    g_fr1002_status = GET_CMD;
                }
                else 
                {
                    __FR1002ERROR_HANDLE();
                }
                break;
            }
            case GET_CMD:
            {
                if (g_mesage_effect_offset == 5)
                {
                    g_fr1002_message.cmd = g_fr1002_receive_buf[g_buf_handle_offset];
                    ++g_mesage_effect_offset;
                    g_fr1002_status = GET_STATE;
                }
                else 
                {
                    __FR1002ERROR_HANDLE();                   
                }
                break;
            }
            case GET_STATE:
            {
                if (g_mesage_effect_offset == 6)
                {
                    g_fr1002_message.cmd = g_fr1002_receive_buf[g_buf_handle_offset];
                    ++g_mesage_effect_offset;                
                    g_fr1002_status = GET_DATA;                    
                }
                else
                {
                    __FR1002ERROR_HANDLE();
                }
                break;
            }
            case GET_DATA:
            {
                if (g_mesage_effect_offset - 7 < g_fr1002_message.data_lenth)
                {
                    g_fr1002_message.data[g_mesage_effect_offset - 7] = 
                    g_fr1002_receive_buf[g_buf_handle_offset];
                    ++g_mesage_effect_offset;
                }
                else 
                {
                    g_fr1002_status = GET_PARITY;
                    g_mesage_effect_offset = 0;
                }
                break;
            }
            case GET_PARITY:
            {               
                g_fr1002_status = VerifyParity(g_fr1002_receive_buf[g_buf_handle_offset],  &g_fr1002_message);
                g_fr1002_message.isend = 1;
                rt_sem_release(g_rf1002_message_end_sem);
                break;
            }
            default:
                __FR1002ERROR_HANDLE();
                break;
            }
            ++g_buf_handle_offset;
        }
        
    }
    
}



void USART3_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void)
{
    g_buf_store_offset %= RF1002_RECEIVE_BUF_MAX;
    g_buf_handle_offset = g_buf_store_offset;
    g_fr1002_receive_buf[g_buf_store_offset] = (uint8_t)USART_ReceiveData(USART3);
    ++g_buf_store_offset;


    if (g_buf_store_offset - g_buf_handle_offset >= 30 || g_buf_store_offset - g_buf_handle_offset <= -70)
        rt_sem_release(g_rf1002_usart_sem);
}
