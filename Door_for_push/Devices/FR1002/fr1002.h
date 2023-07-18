#ifndef USER_TASK_FR1002_H_
#define USER_TASK_FR1002_H_
#include <stdint.h>

void MSG_Send(uint8_t *sendData, uint32_t sendLen);
void FaceMatchMesageSend(void);
void FaceDeletAll(void);
void FaceRegistMessageSend(uint8_t* name, uint8_t lenth);
void FR1002Init(void);

#define GET_HEAD          (0)
#define GET_MESSAGE_TYPE  (1)
#define GET_LENGTH        (2)
#define GET_CMD           (3)
#define GET_STATE         (4)
#define GET_DATA          (5)
#define GET_PARITY        (6)
#define END_RECIEVE       (7)
#define MAX_SIZE          (64)
#define NORM_CMD_TIME_OUT (50)

#define ENROLLMENT_BUTTON (1)
#define DELETE_ALL_BUTTON (2)

#define RF1002_RECEIVE_BUF_MAX 100

typedef enum Fr1002MessageSpecies
{
    k_reply = 0,
    k_note = 1,
    k_image = 2
}Fr1002MessageSpecies;

typedef enum Fr1002Cmd
{
    k_match_face = 0x12,
    k_registe_face = 0x13

}Fr1002Cmd;

typedef struct Fr1002Message
{
    uint16_t data_lenth;
    Fr1002MessageSpecies message_species;
    Fr1002Cmd cmd;
    uint8_t data[64];
    uint8_t parity;
    uint8_t status;
    uint8_t isend;
}Fr1002Message;

typedef struct Fr1002User
{
    uint8_t name[32];
    uint8_t id[2];
}Fr1002User;

extern Fr1002User g_receive_user;
uint8_t VerifyParity(uint8_t data,  Fr1002Message* message);

#endif /* USER_TASK_FR1002_H_ */
