#ifndef THREAD_UI_H_
#define THREAD_UI_H_
#include "LCD/lcd.h"


void ThreadUi(void* args);

void UiPushUiClock(void* args);

int UiPushUiWaitForPassWord(void(* func)(void));

void UiPushUiShowInputPassWord(void);

void UiPushUiWelcomeWindow(void);

void UiPushUiShowError(void);


#include "Functions/Inc/list.h"
int UiPushUiShowMenuList(MenuListManager* list);

#include "Functions/Inc/flash.h"
#include "Threads/Inc/thread_rfid.h"
void UiManageRfid(void);


void UiManageFaceId(void);
extern MenuListManager g_main_list, g_rfid_list, g_rfid_list_manager, g_faceid_list_mamager;

#endif
