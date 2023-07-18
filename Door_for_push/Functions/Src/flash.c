#include "flash.h"

#include "ch32v30x.h"

/* 
    @brief 向flash写入数据
    @param uint32_t address 要写入的flash地址在DATA_FLASH_BASE 和 DATA_FLASH_END 之间且为4的倍数
    @param uint16_t* pbuffer 要写入的数据指针（不支持8字节数据）
    @param uint16_t lenth 数据的长度（按uint16_t算 int为2，short为 1）
    @int is_erease 是否擦除页上已存在的数据（以DATA_FLASH_BASE 为起始， 4KB为一页）
*/
int FlashWrite(uint32_t address, uint16_t* pbuffer, uint16_t lenth, int is_erease)
{
    FLASH_Unlock();
    if ((uint32_t)address > DATA_FLASH_END || (uint32_t)address < DATA_FLASH_BASE)
        return -1;
    volatile int page = ((uint32_t)address - DATA_FLASH_BASE) / FLASH_PAGE_SIZE;
    volatile int pagenum = ((uint32_t)address - DATA_FLASH_BASE + lenth * FLASH_WORD_LENTH) / FLASH_PAGE_SIZE + 1 - page;
    volatile FLASH_Status flash_status = FLASH_COMPLETE;
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    if (is_erease)
    {
        for (int curpage = page; curpage - page < pagenum; ++curpage)
        {
            flash_status = FLASH_ErasePage(curpage * FLASH_PAGE_SIZE + DATA_FLASH_BASE);
            if (flash_status != FLASH_COMPLETE)
            {
                FLASH_Lock();
                return -2;
            }
        }
    }

    for (int i = 0; i < lenth; ++i)
    {
        flash_status = FLASH_ProgramHalfWord((uint32_t)((uint16_t*)address + i), pbuffer[i]);
        if (flash_status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return -2;
        }       
    }
    /* 检查 */
    uint16_t tmp = 0;
    for (int i = 0; i < lenth; ++i)
    {
        tmp = *((volatile uint16_t*)address + i);
        if (tmp != pbuffer[i])
        {
            FLASH_Lock();
            return -3;
        }           
    }
    FLASH_Lock();
    return 0;
}

/* 
    @brief 向flash写入数据
    @param uint32_t address 要读取的flash地址在DATA_FLASH_BASE 和 DATA_FLASH_END 之间且为4的倍数
    @param uint16_t* pbuffer 存储读取的数据的空间（不支持8字节数据）
    @param uint16_t lenth 数据的长度（按uint16_t算 int为2，short为 1）
*/
int FlashRead(uint32_t address, uint16_t* pbuffer, uint16_t lenth)
{
    if ((uint32_t)address > DATA_FLASH_END || (uint32_t)address < DATA_FLASH_BASE)
        return -1;
    if (FLASH->STATR == FLASH_BUSY)
        return -2;
    volatile uint16_t* ptr = (volatile uint16_t*)address;

    for (int i = 0; i < lenth; ++i)
    {
        pbuffer[i] = ptr[i];
    }
    while(FLASH->STATR == FLASH_BUSY);
    return 0;

}


