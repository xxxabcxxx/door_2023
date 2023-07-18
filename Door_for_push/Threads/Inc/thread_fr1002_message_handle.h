#ifndef THREAD_FR1002_MESSAGE_HANDLE_H_
#define THREAD_FR1002_MESSAGE_HANDLE_H_

#include "main.h"
#include "Devices/FR1002/fr1002.h"
extern Fr1002Message g_fr1002_message;

extern uint8_t g_fr1002_receive_buf[RF1002_RECEIVE_BUF_MAX];
extern uint8_t g_buf_store_offset;
extern uint8_t g_buf_handle_offset;
extern uint8_t g_fr1002_status;
extern uint8_t g_mesage_effect_offset;
extern int32_t g_fr1002_error_count;
void ThreadFr1002MessageHandle(void* args);

#endif
