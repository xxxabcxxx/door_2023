#ifndef BEEP_H_
#define BEEP_H_

#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include "drv_gpio.h"


void BeepInit(void);

void PlayOkmusic(void);

void PlayErrmusic(void);

#endif