#ifndef FLASH_H_
#define FLASH_H_
#include <stdint.h>


#define FLASH_MAX_PAGE (16)
#define FLASH_BASE_PTR (0x08000000)
#define DATA_FLASH_BASE (FLASH_BASE_PTR + 0xF000)
#define DATA_FLASH_END (DATA_FLASH_BASE + FLASH_MAX_PAGE * FLASH_PAGE_SIZE)
#define FLASH_PAGE_SIZE (4096)
#define FLASH_WORD_LENTH (16)

#define FLASH_ORIGIN_8 (0x39)
#define FLASH_ORIGIN_16 (0xe339)
#define FLASH_ORIGIN_32 (0xe339e339)

#define FLASH_PASS_WORD (DATA_FLASH_BASE)
#define FLASH_RFID (DATA_FLASH_BASE + FLASH_PAGE_SIZE)
#define FLASH_RFID_OFFSET (2 + 8)
#define FLASH_RECODE (DATA_FLASH_BASE + FLASH_PAGE_SIZE * 2)
#define FLASH_RECODE_OFFSET (2)

/* 
    @brief 向flash写入数据
    @param uint32_t address 要写入的flash地址在DATA_FLASH_BASE 和 DATA_FLASH_END 之间且为4的倍数
    @param uint16_t* pbuffer 要写入的数据指针（不支持8字节数据）
    @param uint16_t lenth 数据的长度（按uint16_t算 int为2，short为 1）
    @int is_erease 是否擦除页上已存在的数据（以DATA_FLASH_BASE 为起始， 4KB为一页）
*/
int FlashWrite(uint32_t address, uint16_t* pbuffer, uint16_t lenth, int is_erease);

/* 
    @brief 向flash写入数据
    @param uint32_t address 要读取的flash地址在DATA_FLASH_BASE 和 DATA_FLASH_END 之间且为4的倍数
    @param uint16_t* pbuffer 存储读取的数据的空间（不支持8字节数据）
    @param uint16_t lenth 数据的长度（按uint16_t算 int为2，short为 1）
*/
int FlashRead(uint32_t address, uint16_t* pbuffer, uint16_t lenth);

#endif
