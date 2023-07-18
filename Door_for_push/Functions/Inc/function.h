#ifndef FUNCTION_H_
#define FUNCTION_H_
#include <stdint.h>

#ifndef __DECLARE
#   define __DECLARE extern
#endif // !__DECLARE

#ifndef TRUE
#   define TRUE (1)
#endif // !TRUE

#ifndef FALSE
#   define FALSE (0)
#endif // !FALSE

#ifndef NULL
#   define NULL ((void* )0)
#endif // !NULL

#ifndef ABS
#   define ABS(a)    ((a) > 0 ? (a) : (-(a)))
#endif // !ABS


#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif // !MIN

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif // !MAX

#ifndef SUCCESS_
#   define SUCCESS_ (1)
#endif // !SUCCESS

#ifndef ERROR_
#   define ERROR_ (0)
#endif // !ERROR_

uint8_t GetKey(void);
void Tim7ForDelayUsInit(void);

#define PASSWORD_LENTH 4

static inline uint8_t NumToChar(int num)
{
    if (num >= 0 && num <= 9)
        return num + 48;
    else    return '!';
}
static inline int CharToNum(uint8_t ch)
{
    int tmp = ch - 48;
    if (tmp >= 0 && tmp <= 9)
        return tmp;
    else return -1;
}
static inline uint8_t* CharToString(uint8_t ch)
{
    static uint8_t buf[3] = { 0, 0, 0 };
    buf[0] = ch;
    return buf;
}


typedef struct PassWord
{
    uint8_t data[PASSWORD_LENTH];
    uint16_t total_lenth;
    uint16_t effective_lenth;
}PassWord;

extern PassWord g_password, g_password_buf;

static inline void InsertPassWord(PassWord* buf, char input)
{
    if (buf->effective_lenth >= PASSWORD_LENTH)
        return;

    buf->data[buf->effective_lenth] = NumToChar(input);
    buf->effective_lenth += 1;
}

static inline void ClearPassWord(PassWord* buf)
{
    buf->effective_lenth = 0;
    for (int i = 0; i < PASSWORD_LENTH; ++i)
    {
        buf->data[i] = 0;
    }
}

static inline void DeletePassWord(PassWord* buf)
{
    if (buf->effective_lenth > 0)
    {
        buf->effective_lenth -= 1;
        buf->data[buf->effective_lenth] = 0;
    }
}

int CheckPassWord(void);

typedef struct RfidData
{
    uint32_t offset;
    uint8_t id[8];
}RfidData;

typedef struct RfidManager
{
    int toal_id_num;
    RfidData* data;
}RfidManager;

extern RfidManager g_rfid_manager;

int CheckIdCard(uint8_t* input_id);
void DeleteIdCard(uint8_t* input);
void AddIdCard(uint8_t* input_id);
void FlashRfidSave(void);

#define MAX_RECODEDATA_NUM 10
#include "rtc/rtc.h"
typedef enum OpenMode
{
    k_by_password,
    k_by_rfid,
    k_by_faceid
}OpenMode;

typedef struct RecodeData
{
    OpenMode mode;
    uint32_t time;
}RecodeData;

typedef struct Recoder
{
    uint32_t recode_num;
    RecodeData *data;
}Recoder;

extern Recoder g_door_recoder;

void FlashSaveRecode(void);
void AddRecoder(Recoder* recode, OpenMode mode, uint32_t time);

void DelayUs(uint32_t us);



#endif
