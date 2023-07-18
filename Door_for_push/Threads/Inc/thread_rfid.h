#ifndef THREAD_RFID_H_
#define THREAD_RFID_H_
#include "function.h"

void ThreadRfid(void* args);
uint8_t* WaitForRfid(void);

typedef enum RfidReadFlag
{
    rfid_judge_error = ERROR_,
    rfid_judge_success = SUCCESS_,
    rfid_read_fail
}RfidReadFlag;


RfidReadFlag ReadRfid(void);

#endif