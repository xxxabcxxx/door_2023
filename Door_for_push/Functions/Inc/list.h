#ifndef LIST_H_
#define LIST_H_
#include <stdint.h>
#include "rtthread.h"


#define MENU_LIST_NAME_MAX_LENTH 16
typedef struct MenuList
{
    uint8_t name[MENU_LIST_NAME_MAX_LENTH];
    uint32_t offset;
    struct MenuList* prev, *next;
}MenuList;


typedef struct MenuListManager
{
    MenuList* phead, * pend;
    uint32_t list_lenth;
    MenuList* cur_list;
}MenuListManager;

static inline void AddMenuListNode(MenuListManager* manager, MenuList* node)
{
    node->offset = manager->list_lenth;
    if (manager->list_lenth == 0)
    {
        node->prev = node->next = node;
        manager->phead = manager->pend = node;
        manager->cur_list = node;
    }
    else
    {
        node->prev = manager->pend;
        node->next = manager->phead;
        manager->pend->next = node;
        manager->phead->prev = node;
        manager->pend = node;
    }
    manager->list_lenth += 1;
}

static inline void DeleteMenuListNode(MenuListManager* manager, uint32_t offset)
{
    offset %= manager->list_lenth;
    MenuList* tmp = manager->phead;
    if (manager->list_lenth == 0)
        return;
    if (manager->list_lenth == 1)
    {
        rt_free(manager->cur_list);
        manager->cur_list = NULL;
        manager->pend = manager->phead = NULL;
        manager->list_lenth = 0;
        return;
    }

    if (offset == 0)
    {
        manager->phead = manager->phead->next;
    }
    if (offset == manager->list_lenth - 1)
    {
        manager->pend = manager->pend->prev;
    }
    uint32_t tmp_offset = offset;
    while (tmp_offset)
    {
        tmp = tmp->next;
        --tmp_offset;
    }
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;
    MenuList* tmpp = tmp->next;

    while (manager->list_lenth - offset - 1)
    {
        tmpp->offset -= 1;
        tmpp = tmpp->next;
        ++offset;
    }
    manager->list_lenth -= 1;
    tmp = manager->cur_list;
    if (manager->cur_list->offset == offset)
    {
        manager->cur_list = manager->cur_list->next;
    }
    rt_free(tmp);
}

static inline void FreeUiList(MenuListManager* manager)
{
    MenuList* tmp = manager->phead;
    while (--manager->list_lenth)
    {
        MenuList* tmpp = tmp->next;
        rt_free(tmp);
        tmp = tmpp;
    }
}

static inline MenuList* CreateMenuListNode(uint8_t* ch)
{
    MenuList* tmp = rt_malloc(sizeof(MenuList));
    for (int i = 0; ch[i] != 0 && i < MENU_LIST_NAME_MAX_LENTH; ++i)
    {
        tmp->name[i] = ch[i];
    }
    return tmp;
}



#endif
